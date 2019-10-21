/* Copyright: Hugh Hyndman 2019 */

/*
 * TODO List:
 *   - support varying number of arguments
 *   - NULLs
 *   - flesh out conversion variants
 *   - improve "no conversion errors"
 *   - kdb+ function to build "create type" given kdb+ meta
 *   - char(1) maps to BPCHAROID (figure it out)
 *   - format and comment code following the PG standard
 *   - document (in MD format)
 *   - test builds on Windows and Linux
 *   - build regression suite
 */

#define KXVER 3
#include "k.h"
#undef closesocket 

#include <postgres.h>
#include <fmgr.h>
#include <utils/builtins.h>
#include <funcapi.h>
#include <access/htup_details.h>
#include <catalog/pg_type_d.h>
#include <utils/guc.h>
#include <utils/timestamp.h>

PG_MODULE_MAGIC;

/* Configuration globals */
static char host[256] = "localhost";
static int port = 5000;
static char userpass[256] = "";

/* Function prototypes */
void _PG_init(void);

Datum conv_bool(K, int);
Datum conv_int2(K, int);
Datum conv_int4(K, int);
Datum conv_int8(K, int);
Datum conv_float4(K, int);
Datum conv_float8(K, int);
Datum conv_char(K, int);
Datum conv_varchar(K, int);
Datum conv_timestamp(K, int);

int   _conv_bool(K, int);
int16 _conv_int2(K, int);
int32 _conv_int4(K, int);
int64 _conv_int8(K, int);
float _conv_float4(K, int);
double _conv_float8(K, int);
uint8 _conv_char(K, int);
int64 _conv_timestamp(K, int);

int findOID(int);
int findName(char *, K);
void safecpy(char *, const char *, size_t);


/* Information needed across calls and stored in the function context */
typedef struct
{
    K   table;
    int *perm;
    int *dtind;
} UIFC;

/* Type dispatch table */

struct
{
    int     typeoid;
    Datum   (*convfn)(K, int);
    bool    isref;
} dt[9] =
{
    { BOOLOID,      conv_bool,      false },
    { INT2OID,      conv_int2,      false },
    { INT4OID,      conv_int4,      false },
    { INT8OID,      conv_int8,      false },
    { FLOAT4OID,    conv_float4,    false },
    { FLOAT8OID,    conv_float8,    false },
    { BPCHAROID,    conv_char,      false },
    { TIMESTAMPOID, conv_timestamp, false },
    { VARCHAROID,   conv_varchar,   true  }
};

/* Return position of type OID in type dispatch table */
int findOID(int oid)
{
    for (int i = 0; i < sizeof(dt)/sizeof(dt[0]); i++)
        if (dt[i].typeoid == oid)
            return i;
    return -1;
}

/* Return position of name in kdb+ column name vector */
int findName(char *f, K nv)
{
    for (int i = 0; i < nv->n; i++)
        if (strcmp(f, kS(nv)[i]) == 0)
            return i;
    return -1;
}

/* A version of strncpy that is safe and always 0-terminates the string */
void safecpy(char *dest, const char *src, size_t n)
{
    strncpy(dest, src, n);
    dest[n - 1] = '\0';
}

/*
 * Initialize configuration variables upon loading of this extension
 */
void _PG_init(void)
{
    const char *p;
    
    if ((p = GetConfigOption("kdb.host", true, false)) != NULL)
        safecpy(host, p, sizeof(host));

    if ((p = GetConfigOption("kdb.port", true, false)) != NULL)
        port = atoi(p);

    if ((p = GetConfigOption("kdb.userpass", true, false)) != NULL)
        safecpy(userpass, p, sizeof(userpass));
}


PG_FUNCTION_INFO_V1(kdb_query);

Datum kdb_query(PG_FUNCTION_ARGS)
{
    FuncCallContext     *funcctx;
    int                  call_cntr;
    int                  max_calls;
    TupleDesc            tupdesc;
    AttInMetadata       *attinmeta;
    K                    table, colnames, values; //!
    int                  natts;
    UIFC                *puifc;
    int                 *perm;
    int                 *dtind;

    /* stuff done only on the first call of the function */
    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext   oldcontext;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /* switch to memory context appropriate for multiple function calls */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* Build a tuple descriptor for our result type */
        TypeFuncClass tfc = get_call_result_type(fcinfo, NULL, &tupdesc);
        if (tfc != TYPEFUNC_COMPOSITE)
            elog(ERROR, "kdb: function must use composite types");

        /* connect to a kdb+ process */
        I handle = khpu(host, port, userpass); 
        if (handle <= 0)
            elog(ERROR, "kdb: socket connection error: %d", handle);

        /* convert query string argument to a cstring */
        S p = text_to_cstring(PG_GETARG_VARCHAR_PP(0));

        /* call kdb+ and retrieve table */
        table = k(handle, p, (K) 0);
        kclose(handle);

        /* ensure result is a simple (unkeyed) table */
        if (!table)
            elog(ERROR, "kdb: network error: %s", strerror(errno));
        else if (-128 == table->t)
        {
            char *p = pstrdup(TX(S, table)); /* need to duplicate error string */
            r0(table); /* before table gets deallocatd */
            elog(ERROR, "kdb: %s", p);
        }
        else if (XT != table->t)
        {
            r0(table);
            elog(ERROR, "result is not a table");
        }

        /* generate attribute metadata needed later to produce tuples */
        attinmeta = TupleDescGetAttInMetadata(tupdesc);
        funcctx->attinmeta = attinmeta;

        /* Number of attributes (columns) in result */
        natts = attinmeta->tupdesc->natts;
        perm = (int *) palloc(natts * sizeof(int));
        dtind = (int *) palloc(natts * sizeof(int));
        colnames = kK(table->k)[0];
        
        for (int i = 0; i < natts; i++)
        {
            int pos = findName(attinmeta->tupdesc->attrs[i].attname.data, colnames);
            if (pos == -1)
                elog(ERROR, "column not found (FIXME)");
            perm[i] = pos;

            pos = findOID(attinmeta->tupdesc->attrs[i].atttypid);
            if (pos == -1)
                elog(ERROR, "unsupport type");
            dtind[i] = pos;
        }

        puifc = (UIFC *) palloc0(sizeof(UIFC));
        puifc->table = table;
        puifc->perm = perm;
        puifc->dtind = dtind;

        funcctx->user_fctx = puifc; /* Keep user information around between calls */
        MemoryContextSwitchTo(oldcontext);
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();

    //! Figure out how to avoid duplicate work
    puifc = (UIFC *) funcctx->user_fctx;
    table = puifc->table;
    perm = puifc->perm;
    dtind = puifc->dtind;

    call_cntr = funcctx->call_cntr;
    max_calls = funcctx->max_calls;
    attinmeta = funcctx->attinmeta;
    natts = attinmeta->tupdesc->natts;

    values = kK(table->k)[1];
    if (call_cntr < kK(values)[0]->n)
    {
        HeapTuple    tuple;
        Datum        result;
        Datum       *dvalues;
        bool        *nulls;

        /* initialize components that make up the tupled (data and null indicators) */
        dvalues = (Datum *) palloc(natts * sizeof(Datum));
        nulls = (bool *) palloc0(natts * sizeof(bool)); 

        /*
         * Iterate through table columns converting from kdb+ format to 
         * Datum format.
         */
        for (int i = 0; i < natts; i++)
        {
            dvalues[i] = (*dt[dtind[i]].convfn)(kK(values)[perm[i]], call_cntr);
        }

        /* create a tuple given a complete row of values */
        tuple = heap_form_tuple(attinmeta->tupdesc, dvalues, nulls);

        /* free up space used by those Datum types that are references */
        for (int i = 0; i < natts; i++)
        {
            if (dt[dtind[i]].isref)
                pfree((void *) dvalues[i]);
        }

        /* 
         * free the rest since a copy of this data was made when the tuple
         * was formed 
         */
        pfree(dvalues); pfree(nulls);

        /* make the tuple into a datum */
        result = HeapTupleGetDatum(tuple);

        SRF_RETURN_NEXT(funcctx, result);
    }
    else /* do when there is no more left */
    {
        /* Free up memory used up by kdb+: r0(result) */
        r0(table);
        SRF_RETURN_DONE(funcctx);
    }
}


/*
 *******************************************************************************
 * Routines that convert kdb+ data types to Postgres data types
 *******************************************************************************
 */

Datum conv_int2(K c, int i)
{
    return Int16GetDatum(_conv_int2(c, i));
}

int16 _conv_int2(K c, int i)
{
    switch (c->t)
    {
        case KC: return kC(c)[i];
        case KH: return kH(c)[i]; 
        default:elog(ERROR, "no conversion");
    }
}

Datum conv_int4(K c, int i)
{
    return Int32GetDatum(_conv_int4(c, i));
}

int32 _conv_int4(K c, int i)
{
    switch (c->t)
    {
        case KC: return kC(c)[i];
        case KH: return kH(c)[i]; 
        case KI: return kI(c)[i];
        default:elog(ERROR, "no conversion");
    }
}

Datum conv_int8(K c, int i)
{
    return Int64GetDatum(_conv_int8(c, i));
}

int64 _conv_int8(K c, int i)
{
    switch (c->t)
    {
        case KC: return kC(c)[i];
        case KH: return kH(c)[i]; 
        case KI: return kI(c)[i];
        case KJ: return kJ(c)[i];
        default:elog(ERROR, "no conversion");
    }
}

Datum conv_float4(K c, int i)
{
    return Float4GetDatum(_conv_float4(c, i));
}

float _conv_float4(K c, int i)
{
    switch (c->t)
    {
        case KH: return kH(c)[i]; 
        case KI: return kI(c)[i];
        case KJ: return kJ(c)[i];
        case KE: return kE(c)[i];
        case KF: return kF(c)[i];
        default:elog(ERROR, "no conversion");
    }
}

Datum conv_float8(K c, int i)
{
    return Float8GetDatum(_conv_float8(c, i));
}

double _conv_float8(K c, int i)
{
    switch (c->t)
    {
        case KH: return kH(c)[i]; 
        case KI: return kI(c)[i];
        case KJ: return kJ(c)[i];
        case KE: return kE(c)[i];
        case KF: return kF(c)[i];
        default:elog(ERROR, "no conversion");
    }
}

//! cleanup
Datum conv_varchar(K c, int i)
{
    if (c->t == 0)
    {
        K p = kK(c)[i];
        char *s = (char *) kC(p);
        int n = p->n;
        return (Datum) cstring_to_text_with_len(s, n);
    } else if (c->t == KC)
    {
        char x = kC(c)[i];
        return (Datum) cstring_to_text_with_len(&x, 1);
    } else if (c->t == KS)
    {
        return (Datum) cstring_to_text(kS(c)[i]);
    }

    elog(ERROR, "no conversion");
}


Datum conv_timestamp(K c, int i)
{
    return TimestampGetDatum(_conv_timestamp(c, i));
}

int64 _conv_timestamp(K c, int i)
{
    switch (c->t)
    {
        case KP: return kJ(c)[i] / 1000;
        //! there are other variants to implement
        default: elog(ERROR, "no conversion");
    }
}

Datum conv_bool(K c, int i)
{
    return BoolGetDatum(_conv_bool(c, i));
}

int _conv_bool(K c, int i)
{
    switch (c->t)
    {
        case KB: return kC(c)[i];
        default: elog(ERROR, "no conversion");
    }
}

Datum conv_char(K c, int i)
{
    return Int8GetDatum(_conv_char(c, i));
}

uint8 _conv_char(K c, int i)
{
    switch (c->t)
    {
        case KC: return kC(c)[i];
        default: elog(ERROR, "no conversion");
    }
}


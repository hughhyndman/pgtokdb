#include "pgtokdb.h"

int		_k2p_bool(K, int);
int16	_k2p_int2(K, int);
int32	_k2p_int4(K, int);
int64	_k2p_int8(K, int);
float	_k2p_float4(K, int);
double	_k2p_float8(K, int);
uint8	_k2p_char(K, int);
int64	_k2p_timestamp(K, int);
int32	_k2p_date(K, int);


K p2k_uuid(Datum x)
{
	return ku(*(U *) DatumGetUUIDP(x));
}

K p2k_bool(Datum x)
{
	return kb(DatumGetBool(x));
}

K p2k_int2(Datum x)
{
	return kh(DatumGetInt16(x));
}

K p2k_int4(Datum x)
{
	return ki(DatumGetInt32(x));
}

K p2k_int8(Datum x)
{
	return kj(DatumGetInt64(x));
}

K p2k_float4(Datum x)
{
	return ke(DatumGetFloat4(x));
}

K p2k_float8(Datum x)
{
	return kf(DatumGetFloat8(x));
}

K p2k_timestamp(Datum x)
{
	return ktj(-KP, 1000 * DatumGetTimestamp(x)); 
}

K p2k_date(Datum x)
{
	return kd(DatumGetInt32(x));
}

K p2k_varchar(Datum x)
{
	return kp(text_to_cstring(DatumGetVarCharPP(x))); 
}

K p2k_char(Datum x)
{
	char *p = text_to_cstring(DatumGetBpCharPP(x));
	return kc(*p);
}

K p2k_bytea(Datum x)
{
	int n = VARSIZE(x) - VARHDRSZ; /* Length of bytea array */
	K bytelist = ktn(KG, n); /* Create a kdb+ byte list */
	memcpy(kG(bytelist), VARDATA(x), n); /* Copy data from Datum to K */
	return bytelist; 
}

K p2k_int8array(Datum x)
{
	return NULL; //! Not implemented
}


/******************/

static const char *k2p_error = "Unable to convert kdb+ column %d to %s";

Datum k2p_bytea(K c, int i)
{
	if (c->t == 0) /* If a list */
	{
		K p = kK(c)[i];
		if (p->t == KG) /* If byte type */
		{
			unsigned char *bytes = (unsigned char *) kG(p);
			int n = p->n; /* Length of byte list */
			int datumlen = VARHDRSZ + n; 
			bytea *datum = (bytea *) palloc(datumlen);
			SET_VARSIZE(datum, datumlen);
			memcpy(VARDATA(datum), bytes, n);
			return PointerGetDatum(datum);
		}
	}
	elog(ERROR, k2p_error, i, "bytea");
}

Datum k2p_uuid(K c, int i)
{
	if (c->t == UU)
	{	
		/* A UUID is 16 bytes; we need allocate space to the result from kdb+ */
		pg_uuid_t *retval = palloc(sizeof(pg_uuid_t));
		Assert(sizeof(pg_uuid_t) == sizeof(U));
		*retval = *(pg_uuid_t *) &kU(c)[i]; 
		return PointerGetDatum(retval);
	}
	elog(ERROR, k2p_error, i, "UUID");
}

Datum k2p_int2(K c, int i)
{
	return Int16GetDatum(_k2p_int2(c, i));
}

int16 _k2p_int2(K c, int i)
{
	switch (c->t)
	{
		case KC: return kC(c)[i];
		case KH: return kH(c)[i]; 
		default:elog(ERROR, k2p_error, i, "smallint (int16))");
	}
}

Datum k2p_int4(K c, int i)
{
	return Int32GetDatum(_k2p_int4(c, i));
}

int32 _k2p_int4(K c, int i)
{
	switch (c->t)
	{
		case KC: return kC(c)[i];
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		default:elog(ERROR, k2p_error, i, "integer (int32)");
	}
}

Datum k2p_int8(K c, int i)
{
	return Int64GetDatum(_k2p_int8(c, i));
}

int64 _k2p_int8(K c, int i)
{
	switch (c->t)
	{
		case KC: return kC(c)[i];
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		case KJ: return kJ(c)[i];
		default:elog(ERROR, k2p_error, i, "bigint (int64)");
	}
}

Datum k2p_float4(K c, int i)
{
	return Float4GetDatum(_k2p_float4(c, i));
}

float _k2p_float4(K c, int i)
{
	switch (c->t)
	{
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		case KJ: return kJ(c)[i];
		case KE: return kE(c)[i];
		case KF: return kF(c)[i];
		default:elog(ERROR, k2p_error, i, "real (float4)");
	}
}

Datum k2p_float8(K c, int i)
{
	return Float8GetDatum(_k2p_float8(c, i));
}

double _k2p_float8(K c, int i)
{
	switch (c->t)
	{
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		case KJ: return kJ(c)[i];
		case KE: return kE(c)[i];
		case KF: return kF(c)[i];
		default:elog(ERROR, k2p_error, i, "double precision (float8)");
	}
}

//! cleanup
Datum k2p_varchar(K c, int i)
{
	if (c->t == 0) /* If a list */
	{
		K p = kK(c)[i];
		if (p->t == KC) /* and a character list */
		{
			char *s = (char *) kC(p);
			int n = p->n;
			return (Datum) cstring_to_text_with_len(s, n);
		} 
		/* Falling through to error at end */
	} 
	else if (c->t == KC) 
	{
		char x = kC(c)[i];
		return (Datum) cstring_to_text_with_len(&x, 1);
	} 
	else if (c->t == KS)
	{
		return (Datum) cstring_to_text(kS(c)[i]);
	}

	elog(ERROR, k2p_error, i, "varchar");
}


Datum k2p_timestamp(K c, int i)
{
	return TimestampGetDatum(_k2p_timestamp(c, i));
}

int64 _k2p_timestamp(K c, int i)
{
	switch (c->t)
	{
		case KP: return kJ(c)[i] / 1000; /* Remove nanoseconds */
		default: elog(ERROR, k2p_error, i, "timestamp");
	}
}

Datum k2p_date(K c , int i)
{
	return Int32GetDatum(_k2p_date(c, i));
}

int32 _k2p_date(K c, int i)
{
	switch (c->t)
	{
		case KD: return kI(c)[i];
		default: elog(ERROR, k2p_error, i, "date");
	}
}

Datum k2p_bool(K c, int i)
{
	return BoolGetDatum(_k2p_bool(c, i));
}

int _k2p_bool(K c, int i)
{
	switch (c->t)
	{
		case KB: return kC(c)[i];
		default: elog(ERROR, k2p_error, i, "boolean");
	}
}

Datum k2p_char(K c, int i)
{
	return k2p_varchar(c, i);
}

/* Convert a kdb+ long list (J), to an int8[] array in Postgres */
Datum k2p_int8array(K c, int i)
{
	if (c->t == 0) /* If a list... */
	{
		K p = kK(c)[i];
		if (p->t == KJ) /* ...of longs (J) */
		{
			int64 *list = (int64 *) kJ(p); /* Pointers to values */
			int n = p->n;  /* Number of elements */

			/* Copy data from K structure to Datum list */
			Datum *data = (Datum *) palloc0(n * sizeof(Datum));
			for (int i = 0; i < n; i++)
				data[i] = Int64GetDatum(list[i]);

			/*
			 * Use PG routine to construct the array. The magic 'd' comes from
			 * inspecting the typalign column of table pg_type, as follows:
			 * 
			 * select typalign, typlen from pg_type where typname like 'int8';
			 */
			ArrayType *array = construct_array(data, n, INT8OID, 8, true, 'd');

			pfree(data);

			return PointerGetDatum(array);
		}
	}
	elog(ERROR, k2p_error, i, "bigint[]");
}

/* Convert a kdb+ int list (I), to an integer[] array in Postgres */
Datum k2p_int4array(K c, int i)
{
	if (c->t == 0) /* If a list... */
	{
		K p = kK(c)[i];
		if (p->t == KI) /* ...of ints (I) */
		{
			int32 *list = (int32 *) kI(p); /* Pointers to values */
			int n = p->n;  /* Number of elements */

			/* Copy data from K structure to Datum list */
			Datum *data = (Datum *) palloc0(n * sizeof(Datum));
			for (int i = 0; i < n; i++)
				data[i] = Int32GetDatum(list[i]);

			/*
			 * Use PG routine to construct the array. The magic 'i' comes from
			 * inspecting the typalign column of table pg_type, as follows:
			 * 
			 * select typalign, typlen from pg_type where typname like 'int4';
			 */
			ArrayType *array = construct_array(data, n, INT4OID, 4, true, 'i');

			pfree(data);

			return PointerGetDatum(array);
		}
	}
	elog(ERROR, k2p_error, i, "integer[]");
}

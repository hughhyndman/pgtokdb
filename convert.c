/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pgtokdb.h"

int		_k2p_bool(K, int, char *);
int16	_k2p_int2(K, int, char *);
int32	_k2p_int4(K, int, char *);
int64	_k2p_int8(K, int, char *);
float	_k2p_float4(K, int, char *);
double	_k2p_float8(K, int, char *);
uint8	_k2p_char(K, int, char *);
int64	_k2p_timestamp(K, int, char *);
int32	_k2p_date(K, int, char *);
Datum 	_k2p_array(K, int, signed char, char *, char *);


K p2k_bool(Datum x)
{
	return kb(DatumGetBool(x));
}

K p2k_uuid(Datum x)
{
	return ku(*(U *) DatumGetUUIDP(x));
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

K p2k_varchar(Datum x)
{
	return kp(text_to_cstring(DatumGetVarCharPP(x))); 
}

K p2k_char(Datum x)
{
	char *p = text_to_cstring(DatumGetBpCharPP(x));
	return kc(*p);
}

K p2k_timestamp(Datum x)
{
	return ktj(-KP, 1000 * DatumGetTimestamp(x)); 
}

K p2k_date(Datum x)
{
	return kd(DatumGetInt32(x));
}

K p2k_bytea(Datum x)
{
	int n = VARSIZE(x) - VARHDRSZ; /* Length of bytea array */
	K bytelist = ktn(KG, n); /* Create a kdb+ byte list */
	memcpy(kG(bytelist), VARDATA(x), n); /* Copy data from Datum to K */
	return bytelist; 
}



static const char *k2p_msg = "Unable to convert kdb+ column '%s' to %s";

Datum k2p_bool(K c, int i, char *n)
{
	return BoolGetDatum(_k2p_bool(c, i, n));
}

int _k2p_bool(K c, int i, char *n)
{
	switch (c->t)
	{
		case KB: return kC(c)[i];
		default: elog(ERROR, k2p_msg, n, "boolean");
	}
}

Datum k2p_uuid(K c, int i, char *n)
{
	if (c->t == UU)
	{	
		/* A UUID is 16 bytes; we need allocate space to the result from kdb+ */
		pg_uuid_t *retval = palloc(sizeof(pg_uuid_t));
		Assert(sizeof(pg_uuid_t) == sizeof(U));
		*retval = *(pg_uuid_t *) &kU(c)[i]; 
		return PointerGetDatum(retval);
	}
	elog(ERROR, k2p_msg, n, "UUID");
}

Datum k2p_int2(K c, int i, char *n)
{
	return Int16GetDatum(_k2p_int2(c, i, n));
}

int16 _k2p_int2(K c, int i, char *n)
{
	switch (c->t)
	{
		case KG: return kG(c)[i];
		case KC: return kC(c)[i];
		case KH: return kH(c)[i]; 
		default:elog(ERROR, k2p_msg, n, "smallint (int16))");
	}
}

Datum k2p_int4(K c, int i, char *n)
{
	return Int32GetDatum(_k2p_int4(c, i, n));
}

int32 _k2p_int4(K c, int i, char *n)
{
	switch (c->t)
	{
		case KG: return kG(c)[i];
		case KC: return kC(c)[i];
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		default:elog(ERROR, k2p_msg, n, "integer (int32)");
	}
}

Datum k2p_int8(K c, int i, char *n)
{
	return Int64GetDatum(_k2p_int8(c, i, n));
}

int64 _k2p_int8(K c, int i, char *n)
{
	switch (c->t)
	{
		case KG: return kG(c)[i];
		case KC: return kC(c)[i];
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		case KJ: return kJ(c)[i];
		default:elog(ERROR, k2p_msg, n, "bigint (int64)");
	}
}

Datum k2p_float4(K c, int i, char *n)
{
	return Float4GetDatum(_k2p_float4(c, i, n));
}

float _k2p_float4(K c, int i, char *n)
{
	switch (c->t)
	{
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		case KJ: return kJ(c)[i];
		case KE: return kE(c)[i];
		case KF: return kF(c)[i];
		default:elog(ERROR, k2p_msg, n, "real (float4)");
	}
}

Datum k2p_float8(K c, int i, char *n)
{
	return Float8GetDatum(_k2p_float8(c, i, n));
}

double _k2p_float8(K c, int i, char *n)
{
	switch (c->t)
	{
		case KH: return kH(c)[i]; 
		case KI: return kI(c)[i];
		case KJ: return kJ(c)[i];
		case KE: return kE(c)[i];
		case KF: return kF(c)[i];
		default:elog(ERROR, k2p_msg, n, "double precision (float8)");
	}
}

//! cleanup
Datum k2p_varchar(K c, int i, char *n)
{
	if (c->t == 0) /* If a list */
	{
		K p = kK(c)[i];
		if (p->t == KC) /* and a character list */
		{
			char *s = (char *) kC(p);
			int l = p->n;
			return (Datum) cstring_to_text_with_len(s, l);
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

	elog(ERROR, k2p_msg, n, "varchar");
}

Datum k2p_char(K c, int i, char *n)
{
	return k2p_varchar(c, i, n);
}

Datum k2p_timestamp(K c, int i, char *n)
{
	return TimestampGetDatum(_k2p_timestamp(c, i, n));
}

int64 _k2p_timestamp(K c, int i, char *n)
{
	switch (c->t)
	{
		case KP: return kJ(c)[i] / 1000; /* Remove nanoseconds */
		default: elog(ERROR, k2p_msg, n, "timestamp");
	}
}

Datum k2p_date(K c, int i, char *n)
{
	return Int32GetDatum(_k2p_date(c, i, n));
}

int32 _k2p_date(K c, int i, char *n)
{
	switch (c->t)
	{
		case KD: return kI(c)[i];
		default: elog(ERROR, k2p_msg, n, "date");
	}
}

Datum k2p_bytea(K c, int i, char *n)
{
	if (c->t == 0) /* If a list */
	{
		K p = kK(c)[i];
		if (p->t == KG) /* If byte type */
		{
			unsigned char *bytes = (unsigned char *) kG(p);
			int l = p->n; /* Length of byte list */
			int datumlen = VARHDRSZ + l; 
			bytea *datum = (bytea *) palloc(datumlen);
			SET_VARSIZE(datum, datumlen);
			memcpy(VARDATA(datum), bytes, l);
			return PointerGetDatum(datum);
		}
	}
	elog(ERROR, k2p_msg, n, "bytea");
}

/* Convert a kdb+ short list (H), to an smallint[] array in Postgres */
Datum k2p_int2array(K c, int i, char *n)
{
	return _k2p_array(c, i, KH, n, "smallint[]");
}

/* Convert a kdb+ int list (I), to an integer[] array in Postgres */
Datum k2p_int4array(K c, int i, char *n)
{
	return _k2p_array(c, i, KI, n, "integer[]");
}

/* Convert a kdb+ long list (J), to an bigint[] array in Postgres */
Datum k2p_int8array(K c, int i, char *n)
{
	return _k2p_array(c, i, KJ, n, "bigint[]");
}

/* Convert a kdb+ real list (E), to an real[] array in Postgres */
Datum k2p_float4array(K c, int i, char *n)
{
	return _k2p_array(c, i, KE, n, "real[]");
}

/* Convert a kdb+ float list (F), to an double precision[] array in Postgres */
Datum k2p_float8array(K c, int i, char *n)
{
	return _k2p_array(c, i, KF, n, "double precision[]");
}

/*
 * Convert row of a table column (containing lists) to a Postgres array. 
 *
 * klol   - kdb+ list of lists, representing a table column: ([] jj:(1 2;3 4 5))
 * krow   - Index into above (specifies a table row)
 * ktype  - Mandatory type of kdb+ list
 * kname  - Name of kdb+ column for error reporting
 * pgtype - Postgres data type name for error reporting
 */
Datum _k2p_array(K klol, int krow, signed char ktype, char *kname, char *pgtype) 
{
	Oid 	elmtype;
	int 	elmlen;
	char 	elmalign;
	int 	i;

	if (klol->t != 0) /* It must be a list */
		elog(ERROR, k2p_msg, kname, pgtype);

	K list = kK(klol)[krow]; /* Get specific row, which points to a list */
	if (list->t != ktype)
		elog(ERROR, k2p_msg, kname, pgtype);

	int arrlen = list->n;  /* Number of elements */
	Datum *data = (Datum *) palloc0(arrlen * sizeof(Datum));

	switch (list->t)
	{
		case KH:
			elmtype = INT2OID; elmlen = sizeof(int16); elmalign = 's';
			for (i = 0; i < arrlen; i++)
				data[i] = Int16GetDatum(kH(list)[i]);
			break;		

		case KI:
			elmtype = INT4OID; elmlen = sizeof(int32); elmalign = 'i';
			for (i = 0; i < arrlen; i++)
				data[i] = Int32GetDatum(kI(list)[i]);
			break;

		case KJ:
			elmtype = INT8OID; elmlen = sizeof(int64); elmalign = 'd';
			for (i = 0; i < arrlen; i++)
				data[i] = Int64GetDatum(kJ(list)[i]);
			break;

		case KE:
			elmtype = FLOAT4OID; elmlen = sizeof(float4); elmalign = 'i';
			for (i = 0; i < arrlen; i++)
				data[i] = Float4GetDatum(kE(list)[i]);
			break;

		case KF:
			elmtype = FLOAT8OID; elmlen = sizeof(float8); elmalign = 'd';
			for (i = 0; i < arrlen; i++)
				data[i] = Float8GetDatum(kF(list)[i]);
			break;

		default:
			elog(ERROR, k2p_msg, kname, pgtype);
	}

	ArrayType *array = construct_array(data, arrlen, elmtype, elmlen, true, elmalign);
	pfree(data);
	return PointerGetDatum(array);
}

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

static const char *k2p_error = "Unable to convert kdb+ column '%s' to %s";

Datum k2p_bool(K c, int i, char *n)
{
	return BoolGetDatum(_k2p_bool(c, i, n));
}

int _k2p_bool(K c, int i, char *n)
{
	switch (c->t)
	{
		case KB: return kC(c)[i];
		default: elog(ERROR, k2p_error, n, "boolean");
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
	elog(ERROR, k2p_error, n, "UUID");
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
		default:elog(ERROR, k2p_error, n, "smallint (int16))");
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
		default:elog(ERROR, k2p_error, n, "integer (int32)");
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
		default:elog(ERROR, k2p_error, n, "bigint (int64)");
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
		default:elog(ERROR, k2p_error, n, "real (float4)");
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
		default:elog(ERROR, k2p_error, n, "double precision (float8)");
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

	elog(ERROR, k2p_error, n, "varchar");
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
		default: elog(ERROR, k2p_error, n, "timestamp");
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
		default: elog(ERROR, k2p_error, n, "date");
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
	elog(ERROR, k2p_error, n, "bytea");
}

/* Convert a kdb+ int list (I), to an integer[] array in Postgres */
Datum k2p_int4array(K c, int i, char *n)
{
	if (c->t == 0) /* If a list... */
	{
		K p = kK(c)[i];
		if (p->t == KI) /* ...of ints (I) */
		{
			int32 *list = (int32 *) kI(p); /* Pointers to values */
			int l = p->n;  /* Number of elements */

			/* Copy data from K structure to Datum list */
			Datum *data = (Datum *) palloc0(l * sizeof(Datum));
			for (int i = 0; i < l; i++)
				data[i] = Int32GetDatum(list[i]);

			/*
			 * Use PG routine to construct the array. The magic 'i' comes from
			 * inspecting the typalign column of table pg_type, as follows:
			 * 
			 * select typalign, typlen from pg_type where typname like 'int4';
			 */
			ArrayType *array = construct_array(data, l, INT4OID, 4, true, 'i');

			pfree(data);

			return PointerGetDatum(array);
		}
	}
	elog(ERROR, k2p_error, n, "integer[]");
}

/* Convert a kdb+ long list (J), to an int8[] array in Postgres */
Datum k2p_int8array(K c, int i, char *n)
{
	if (c->t == 0) /* If a list... */
	{
		K p = kK(c)[i];
		if (p->t == KJ) /* ...of longs (J) */
		{
			int64 *list = (int64 *) kJ(p); /* Pointers to values */
			int l = p->n;  /* Number of elements */

			/* Copy data from K structure to Datum list */
			Datum *data = (Datum *) palloc0(l * sizeof(Datum));
			for (int i = 0; i < l; i++)
				data[i] = Int64GetDatum(list[i]);

			/*
			 * Use PG routine to construct the array. The magic 'd' comes from
			 * inspecting the typalign column of table pg_type, as follows:
			 * 
			 * select typalign, typlen from pg_type where typname like 'int8';
			 */
			ArrayType *array = construct_array(data, l, INT8OID, 8, true, 'd');

			pfree(data);

			return PointerGetDatum(array);
		}
	}
	elog(ERROR, k2p_error, n, "bigint[]");
}

/* Convert a kdb+ real list (E), to an real[] array in Postgres */
Datum k2p_float4array(K c, int i, char *n)
{
	if (c->t == 0) /* If a list... */
	{
		K p = kK(c)[i];
		if (p->t == KE) /* ...of reals (E) */
		{
			float *list = (float *) kE(p); /* Pointers to values */
			int l = p->n;  /* Number of elements */

			/* Copy data from K structure to Datum list */
			Datum *data = (Datum *) palloc0(l * sizeof(Datum));
			for (int i = 0; i < l; i++)
				data[i] = Float4GetDatum(list[i]);

			/*
			 * Use PG routine to construct the array. The magic 'i' comes from
			 * inspecting the typalign column of table pg_type, as follows:
			 * 
			 * select typalign, typlen from pg_type where typname like 'float4';
			 */
			ArrayType *array = construct_array(data, l, FLOAT4OID, 4, true, 'i');

			pfree(data);

			return PointerGetDatum(array);
		}
	}
	elog(ERROR, k2p_error, n, "real[]");
}

/* Convert a kdb+ real list (E), to an real[] array in Postgres */
Datum k2p_float8array(K c, int i, char *n)
{
	if (c->t == 0) /* If a list... */
	{
		K p = kK(c)[i];
		if (p->t == KF) /* ...of floats (F) */
		{
			float8 *list = (float8 *) kF(p); /* Pointers to values */
			int l = p->n;  /* Number of elements */

			/* Copy data from K structure to Datum list */
			Datum *data = (Datum *) palloc0(l * sizeof(Datum));
			for (int i = 0; i < l; i++)
				data[i] = Float8GetDatum(list[i]);

			/*
			 * Use PG routine to construct the array. The magic 'i' comes from
			 * inspecting the typalign column of table pg_type, as follows:
			 * 
			 * select typalign, typlen from pg_type where typname like 'float8';
			 */
			ArrayType *array = construct_array(data, l, FLOAT8OID, 8, true, 'd');

			pfree(data);

			return PointerGetDatum(array);
		}
	}
	elog(ERROR, k2p_error, n, "double precision[]");
}



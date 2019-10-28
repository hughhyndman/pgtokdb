/* Copyright: Hugh Hyndman 2019 */

#include "pgtokdb.h"

int		_k2p_bool(K, int);
int16	_k2p_int2(K, int);
int32	_k2p_int4(K, int);
int64	_k2p_int8(K, int);
float	_k2p_float4(K, int);
double	_k2p_float8(K, int);
uint8	_k2p_char(K, int);
int64	_k2p_timestamp(K, int);


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

K p2k_varchar(Datum x)
{
	//! This isn't optimal. I should be able to get the string without going
	//! through all the complications that text_to_cstring is performing.
	return kp(text_to_cstring(DatumGetVarCharPP(x))); 
}


/******************/

static const char *k2p_error = "Unable to convert kdb+ column %d to %s";

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
		case KP: return kJ(c)[i] / 1000; //! Test this!
		//! there are other variants to implement 
		default: elog(ERROR, k2p_error, i, "timestamp");
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
	return Int8GetDatum(_k2p_char(c, i));
}

uint8 _k2p_char(K c, int i)
{
	switch (c->t)
	{
		case KC: return kC(c)[i];
		default: elog(ERROR, k2p_error, i, "char"); //! not sure here
	}
}
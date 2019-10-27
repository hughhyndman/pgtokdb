#ifndef PGTOKDB_H
#define PGTOKDB_H

#define KXVER 3
#include "k.h"
#undef closesocket 

#include <postgres.h>
#include <catalog/pg_type_d.h>
#include <utils/timestamp.h>
#include <utils/builtins.h>

Datum k2p_bool(K, int);
Datum k2p_int2(K, int);
Datum k2p_int4(K, int);
Datum k2p_int8(K, int);
Datum k2p_float4(K, int);
Datum k2p_float8(K, int);
Datum k2p_char(K, int);
Datum k2p_varchar(K, int);
Datum k2p_timestamp(K, int);

K p2k_bool(Datum);
K p2k_int2(Datum);
K p2k_int4(Datum);
K p2k_int8(Datum);
K p2k_float4(Datum);
K p2k_float8(Datum);
K p2k_varchar(Datum);
K p2k_timestamp(Datum);

#endif /* PKGTOKDB_H */
#ifndef PGTOKDB_H
#define PGTOKDB_H

#include <postgres.h>
#include <catalog/pg_type_d.h>
#include <utils/timestamp.h>
#include <utils/builtins.h>
#include <utils/uuid.h>
#include <utils/array.h>

#define KXVER 3
#include "k.h"

/*
 * Redefine this PG_FUNCTION_INFO_V1 (found in PG's include\fmgr.h) so that 
 * funcname is declared with PGDLLEXPORT -- which is required for a Windows build.
 */
#define PG_FUNCTION_INFO_CUSTOM(funcname) \
extern PGDLLEXPORT Datum funcname(PG_FUNCTION_ARGS); \
extern PGDLLEXPORT const Pg_finfo_record * CppConcat(pg_finfo_,funcname)(void); \
const Pg_finfo_record * \
CppConcat(pg_finfo_,funcname) (void) \
{ \
	static const Pg_finfo_record my_finfo = { 1 }; \
	return &my_finfo; \
} \
extern int no_such_variable


Datum k2p_uuid(K, int);
Datum k2p_bool(K, int);
Datum k2p_int2(K, int);
Datum k2p_int4(K, int);
Datum k2p_int8(K, int);
Datum k2p_float4(K, int);
Datum k2p_float8(K, int);
Datum k2p_varchar(K, int);
Datum k2p_timestamp(K, int);
Datum k2p_char(K, int);
Datum k2p_date(K, int);
Datum k2p_bytea(K, int);
Datum k2p_int8array(K, int);
Datum k2p_int4array(K, int);

K p2k_uuid(Datum);
K p2k_bool(Datum);
K p2k_int2(Datum);
K p2k_int4(Datum);
K p2k_int8(Datum);
K p2k_float4(Datum);
K p2k_float8(Datum);
K p2k_varchar(Datum);
K p2k_timestamp(Datum);
K p2k_char(Datum);
K p2k_date(Datum);
K p2k_bytea(Datum);
K p2k_int8array(Datum);


#endif /* PKGTOKDB_H */
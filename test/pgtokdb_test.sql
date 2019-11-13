

\echo Creating test schema: pgtokdb_test
SET client_min_messages = 'ERROR';
\pset pager

-- Create all test artifacts inside a temporary schema. It is easy to clean up
-- when we are done by doing a "cascade" drop
drop schema if exists pgtokdb_test cascade;
create schema pgtokdb_test;
set search_path to pgtokdb_test;

\echo '************** Happy Path Testing **************'

\echo ** Test01: Simple connectivity 
create type test01_t as (j bigint);
create function test01(varchar, integer) returns setof test01_t as 'pgtokdb', 'getset' language c;
select * from test01('test01', 1);

\echo ** Test02: All types returned
create type test02_t as (
	b boolean,
	x smallint, -- up-casting byte to small integer 
	g UUID,
	h smallint, 
	i integer, 
	j bigint, 
	e real, 
	f double precision, 
	c char,
	s varchar, 
	p timestamp, 
	d date,
	cc varchar,
	hh smallint[],
	ii integer[],
	jj bigint[],
	ee real[],
	ff double precision[]
	);
create function test02(varchar, integer) returns setof test02_t as 'pgtokdb', 'getset' language c;
select * from test02('test02', 4);

\echo ** Test03: smallint (h) passed as argument and returned as result
create type test03_t as (h smallint);
create function test03(varchar, smallint) returns setof test03_t as 'pgtokdb', 'getset' language c;
select * from test03('test03', cast(31 as smallint));

\echo ** Test04: All supported types passed as arguments
create type test04_t as (res boolean);
create function test04(varchar, boolean, smallint, integer, bigint, real, 
	double precision, timestamp, varchar) returns setof test04_t as 'pgtokdb', 'getset' language c;
select * from test04('test04', true, cast(1 as smallint), 2, cast(3 as bigint), 
	cast(4.5 as float4), cast(6.7 as float8), cast(now() as timestamp), 
	'a varchar string');

\echo ** Test05: char (c) passed as argument and returned as result
create type test05_t as (c char);
create function test05(varchar, char) returns setof test05_t as 'pgtokdb', 'getset' language c;
select * from test05('test05', 'T');

\echo ** Test06: timestamp (p) with/without timezone support
create type test06_t as (p1 timestamp, p2 timestamptz);
create function test06(varchar, timestamp, timestamptz) returns setof test06_t 
	as 'pgtokdb', 'getset' language c;
select * from test06('test06', cast(now() as timestamp), now());

\echo ** Test07: date (d) type support
create type test07_t as (d date);
create function test07(varchar, date) returns setof test07_t as 'pgtokdb', 'getset' language c;
select * from test07('test07', '2019-09-01');

\echo ** Test08: UUID (g) type support
create type test08_t as (g UUID); 
create function test08(varchar, UUID) returns setof test08_t as 'pgtokdb', 'getset' language c;
select * from test08('test08', 'A0EEBC99-9C0B-4EF8-BB6D-6BB9BD380A11');

\echo ** Test09: varchar (s) type support
create type test09_t as (s varchar);
create function test09(varchar, integer) returns setof test09_t as 'pgtokdb', 'getset' language c;
select * from test09('test09', 3);

\echo ** Test10: text (C) type support
create type test10_t as (tt text); 
create function test10(varchar, text) returns setof test10_t as 'pgtokdb', 'getset' language c;
select * from test10('test10', 'Here is some text');

\echo ** Test11: bytea (X) type support
create type test11_t as (xx bytea); 
create function test11(varchar, bytea) returns setof test11_t as 'pgtokdb', 'getset' language c;
select * from test11('test11', '\xBAADF00D');

\echo ** Test12: smallint[] (H) type support
create type test12_t as (hh smallint[]);
create function test12(varchar, integer) returns setof test12_t as 'pgtokdb', 'getset' language c;
select * from test12('test12', 4);

\echo ** Test13: bigint[] (J) type support
create type test13_t as (jj bigint[]);
create function test13(varchar, integer) returns setof test13_t as 'pgtokdb', 'getset' language c;
select * from test13('test13', 3);

\echo ** Test14: integer[] (I) type support
create type test14_t as (ii integer[]);
create function test14(varchar, integer) returns setof test14_t as 'pgtokdb', 'getset' language c;
select * from test14('test14', 3);

\echo ** Test15: real[] (E) type support
create type test15_t as (ee real[]);
create function test15(varchar, integer) returns setof test15_t as 'pgtokdb', 'getset' language c;
select * from test15('test15', 3);

\echo ** Test16: double precision[] (F) type support
create type test16_t as (ff double precision[]);
create function test16(varchar, integer) returns setof test16_t as 'pgtokdb', 'getset' language c;
select * from test16('test16', 3);

\echo ** Test17: Casting up kdb+ type h (short) to Postgres integer, bigint, real, and double precision
create type test17_t as (i integer, j bigint, e real, f double precision);
create function test17(varchar, smallint) returns setof test17_t as 'pgtokdb', 'getset' language c;
select * from test17('test17', cast(32767 as smallint));

\echo ** Test18: Casting up kdb+ type i (int) to Postgres bigint, real, and double precision
create type test18_t as (j bigint, e real, f double precision);
create function test18(varchar, integer) returns setof test18_t as 'pgtokdb', 'getset' language c;
select * from test18('test18', 2147483646);

\echo ** Test19: Casting up kdb+ type j (long) to Postgres real and double precision
create type test19_t as (e real, f double precision);
create function test19(varchar, bigint) returns setof test19_t as 'pgtokdb', 'getset' language c;
select * from test19('test19', 9223372036854775806);

\echo ** Test20: Casting up kdb+ type e (real) to Postgres double precision
create type test20_t as (f double precision);
create function test20(varchar, real) returns setof test20_t as 'pgtokdb', 'getset' language c;
select * from test20('test20', 5.5);


\echo '************** Exception Path Testing **************'

\echo ** Test21: Error on kdb+ not returning unkeyed table
create type test21_t as (j bigint);
create function test21(varchar) returns setof test21_t as 'pgtokdb', 'getset' language c;
select * from test21('test21[]');

\echo ** Test22: Unsupported argument types
create type test22_t as (j bigint);
create function test22(varchar, money) returns setof test22_t as 'pgtokdb', 'getset' language c;
select * from test22('test22', cast(9.99 as money));

\echo ** Test23: Unsupported return types
create type test23_t as (m money);
create function test23(varchar) returns setof test23_t as 'pgtokdb', 'getset' language c;
select * from test23('test23[]');

\echo ** Test24: Missing (mismatched) column name
create type test24_t as (jcol bigint);
create function test24(varchar) returns setof test24_t as 'pgtokdb', 'getset' language c;
select * from test24('test24[]');

\echo ** Test25: No conversion to smallint
create type test25_t as (c smallint);
create function test25(varchar) returns setof test25_t as 'pgtokdb', 'getset' language c;
select * from test25('test25[]');

\echo ** Test26: No conversion to integer
create type test26_t as (c integer);
create function test26(varchar) returns setof test26_t as 'pgtokdb', 'getset' language c;
select * from test26('test26[]');

\echo ** Test27: No conversion to bigint
create type test27_t as (c bigint);
create function test27(varchar) returns setof test27_t as 'pgtokdb', 'getset' language c;
select * from test27('test27[]');

\echo ** Test28: No conversion to real
create type test28_t as (c real);
create function test28(varchar) returns setof test28_t as 'pgtokdb', 'getset' language c;
select * from test28('test28[]');

\echo ** Test29: No conversion to double precision
create type test29_t as (c double precision);
create function test29(varchar) returns setof test29_t as 'pgtokdb', 'getset' language c;
select * from test29('test29[]');

\echo ** Test30: No conversion to varchar
create type test30_t as (c varchar);
create function test30(varchar) returns setof test30_t as 'pgtokdb', 'getset' language c;
select * from test30('test30[]');

\echo ** Test31: No conversion to boolean
create type test31_t as (c boolean);
create function test31(varchar) returns setof test31_t as 'pgtokdb', 'getset' language c;
select * from test31('test31[]');

\echo ** Test32: No conversion to bytea
create type test32_t as (c bytea);
create function test32(varchar) returns setof test32_t as 'pgtokdb', 'getset' language c;
select * from test32('test32[]');

\echo ** Test33: No conversion to UUID
create type test33_t as (c UUID);
create function test33(varchar) returns setof test33_t as 'pgtokdb', 'getset' language c;
select * from test33('test33[]');

\echo ** Test34: No conversion to timestamp
create type test34_t as (c timestamp);
create function test34(varchar) returns setof test34_t as 'pgtokdb', 'getset' language c;
select * from test34('test34[]');

\echo ** Test35: No conversion to date
create type test35_t as (c date);
create function test35(varchar) returns setof test35_t as 'pgtokdb', 'getset' language c;
select * from test35('test35[]');

\echo ** Test36: No conversion to Postgres array[] from column of atoms
create type test36_t as (ii integer[]);
create function test36(varchar) returns setof test36_t as 'pgtokdb', 'getset' language c;
select * from test36('test36[]');

\echo ** Test37: Conversion type mismatch between array types 
create type test37_t as (ii integer[]);
create function test37(varchar) returns setof test37_t as 'pgtokdb', 'getset' language c;
select * from test37('test37[]');

\echo ** Test38: Unsupported kdb+ array type
create type test38_t as (ii integer[]);
create function test38(varchar) returns setof test38_t as 'pgtokdb', 'getset' language c;
select * from test38('test38[]');

\echo ** Test39: Kdb+ error
create type test39_t as (j bigint);
create function test39(varchar) returns setof test39_t as 'pgtokdb', 'getset' language c;
select * from test39('test39[]');

\echo ** Test40: Function has no arguments
create type test40_t as (j bigint);
create function test40() returns setof test40_t as 'pgtokdb', 'getset' language c;
select * from test40();

\echo ** Test41: First argument to function must be a varchar
create type test41_t as (j bigint);
create function test41(integer) returns setof test41_t as 'pgtokdb', 'getset' language c;
select * from test41(33);

\echo ** Test42: Exceeded maximum number of arguments
create type test42_t as (j bigint);
create function test42(
	varchar, integer, integer, integer,
	integer, integer, integer, integer,
	integer, integer) returns setof test42_t as 'pgtokdb', 'getset' language c;
select * from test42('test42[]', 1, 2, 3, 4, 5, 6, 7, 8, 9);

\echo '************** Performance Testing **************'

\echo ** Test43: Retrieving 100,000 wide (1000+256+16 bytes) row requiring additional pallocs
create type test43_t as (cc varchar, xx bytea, g UUID);
create function test43(varchar, integer) returns setof test43_t as 'pgtokdb', 'getset' language c;
\timing on
select count(*) from test43('test43', 100000);
\timing off

\echo ** Test44: Retrieving 10 million narrow rows (4 bytes)
create type test44_t as (i integer);
create function test44(varchar, integer) returns setof test44_t as 'pgtokdb', 'getset' language c;
\timing on
select count(*) from test44('test44', 1000000);
\timing off

-- Get rid of all testing artifacts
\echo Dropping test schema: pgtokdb_test
drop schema pgtokdb_test cascade;
set search_path to public;
set client_min_messages = 'notice'

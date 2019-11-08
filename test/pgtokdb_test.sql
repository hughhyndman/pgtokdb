
\echo Creating test schema: pgtokdb_test
SET client_min_messages = 'ERROR';

-- Create all test artifacts inside a temporary schema. It is easy to clean up
drop schema if exists pgtokdb_test cascade;
create schema pgtokdb_test;
set search_path to pgtokdb_test;

\echo '************** Happy Path Testing **************'

\echo ** Test01: Simple connectivity 
create type test01_t as (j bigint);
create function test01(varchar, integer) returns setof test01_t as 'pgtokdb', 'getset' language c;
select * from test01('test01', 1);

\echo ** Test02: All types returned
create type test02_t as (b boolean, h smallint, i integer, j bigint, e real, 
	f double precision, p timestamp, c char, cc varchar);
create function test02(varchar, integer) returns setof test02_t as 'pgtokdb', 'getset' language c;
select * from test02('test02', 1);

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

\echo ** Test09: text (C) type support
create type test09_t as (tt text); 
create function test09(varchar, text) returns setof test09_t as 'pgtokdb', 'getset' language c;
select * from test09('test09', 'Here is some text');

\echo ** Test10: bytea (X) type support
create type test10_t as (xx bytea); 
create function test10(varchar, bytea) returns setof test10_t as 'pgtokdb', 'getset' language c;
select * from test10('test10', '\xBAADF00D');

\echo ** Test11: Casting up kdb+ type h (short) to Postgres integer, bigint, real, and double precision
create type test11_t as (i integer, j bigint, e real, f double precision);
create function test11(varchar, smallint) returns setof test11_t as 'pgtokdb', 'getset' language c;
select * from test11('test11', cast(32767 as smallint));

\echo ** Test12: Casting up kdb+ type i (int) to Postgres bigint, real, and double precision
create type test12_t as (j bigint, e real, f double precision);
create function test12(varchar, integer) returns setof test12_t as 'pgtokdb', 'getset' language c;
select * from test12('test12', 2147483646);

\echo ** Test13: Casting up kdb+ type j (long) to Postgres real and double precision
create type test13_t as (e real, f double precision);
create function test13(varchar, bigint) returns setof test13_t as 'pgtokdb', 'getset' language c;
select * from test13('test13', 9223372036854775806);

\echo ** Test14: Casting up kdb+ type e (real) to Postgres double precision
create type test14_t as (f double precision);
create function test14(varchar, real) returns setof test14_t as 'pgtokdb', 'getset' language c;
select * from test14('test14', 5.5);


\echo '************** Exception Path Testing **************'

\echo ** Test15: Error on kdb+ not returning unkeyed table
create type test15_t as (j bigint);
create function test15(varchar) returns setof test15_t as 'pgtokdb', 'getset' language c;
select * from test15('til 10');
select * from test15('test15[]');

\echo ** Test16: Unsupported argument types
create type test16_t as (j bigint);
create function test16(varchar, money) returns setof test16_t as 'pgtokdb', 'getset' language c;
select * from test16('test16', cast(9.99 as money));

\echo ** Test17: Unsupported return types
create type test17_t as (m money);
create function test17(varchar) returns setof test17_t as 'pgtokdb', 'getset' language c;
select * from test17('test17[]');

\echo ** Test18: Missing (mismatched) column name
create type test18_t as (jcol bigint);
create function test18(varchar) returns setof test18_t as 'pgtokdb', 'getset' language c;
select * from test18('test18[]');

\echo ** Test19: No conversion to smallint
create type test19_t as (c smallint);
create function test19(varchar) returns setof test19_t as 'pgtokdb', 'getset' language c;
select * from test19('test19[]');

\echo ** Test20: No conversion to integer
create type test20_t as (c integer);
create function test20(varchar) returns setof test20_t as 'pgtokdb', 'getset' language c;
select * from test20('test20[]');

\echo ** Test21: No conversion to bigint
create type test21_t as (c bigint);
create function test21(varchar) returns setof test21_t as 'pgtokdb', 'getset' language c;
select * from test21('test21[]');

\echo ** Test22: No conversion to real
create type test22_t as (c real);
create function test22(varchar) returns setof test22_t as 'pgtokdb', 'getset' language c;
select * from test22('test22[]');

\echo ** Test23: No conversion to double precision
create type test23_t as (c double precision);
create function test23(varchar) returns setof test23_t as 'pgtokdb', 'getset' language c;
select * from test23('test23[]');

\echo ** Test24: No conversion to varchar
create type test24_t as (c varchar);
create function test24(varchar) returns setof test24_t as 'pgtokdb', 'getset' language c;
select * from test24('test24[]');

\echo ** Test25: No conversion to boolean
create type test25_t as (c boolean);
create function test25(varchar) returns setof test25_t as 'pgtokdb', 'getset' language c;
select * from test25('test25[]');

\echo ** Test26: No conversion to bytea
create type test26_t as (c bytea);
create function test26(varchar) returns setof test26_t as 'pgtokdb', 'getset' language c;
select * from test26('test26[]');

\echo ** Test27: No conversion to UUID
create type test27_t as (c UUID);
create function test27(varchar) returns setof test27_t as 'pgtokdb', 'getset' language c;
select * from test27('test27[]');

\echo ** Test28: No conversion to timestamp
create type test28_t as (c timestamp);
create function test28(varchar) returns setof test28_t as 'pgtokdb', 'getset' language c;
select * from test28('test28[]');

\echo ** Test29: No conversion to date
create type test29_t as (c date);
create function test29(varchar) returns setof test29_t as 'pgtokdb', 'getset' language c;
select * from test29('test29[]');

--//! Check if any types are missing!!!

\echo ** Test30: Kdb+ error
create type test30_t as (j bigint);
create function test30(varchar) returns setof test30_t as 'pgtokdb', 'getset' language c;
select * from test30('test30[]');

\echo ** Test31: Function has no arguments
create type test31_t as (j bigint);
create function test31() returns setof test31_t as 'pgtokdb', 'getset' language c;
select * from test31();

\echo ** Test32: First argument to function must be a varchar
create type test32_t as (j bigint);
create function test32(integer) returns setof test32_t as 'pgtokdb', 'getset' language c;
select * from test32(33);

\echo ** Test33: Exceeded maximum number of arguments
create type test33_t as (j bigint);
create function test33(
	varchar, integer, integer, integer,
	integer, integer, integer, integer,
	integer, integer) returns setof test33_t as 'pgtokdb', 'getset' language c;
select * from test33('test33[]', 1, 2, 3, 4, 5, 6, 7, 8, 9);

\echo '************** Performance Testing **************'
\timing on

\echo ** Test34: Retrieving 100,000 wide rows (1000+256+16 bytes) requiring additional pallocs
create type test34_t as (cc varchar, xx bytea, g UUID);
create function test34(varchar, integer) returns setof test34_t as 'pgtokdb', 'getset' language c;
select count(*) from test34('test34', 100000);

\echo ** Test35: Retrieving 10 million narrow rows (4 bytes)
create type test35_t as (i integer);
create function test35(varchar, integer) returns setof test35_t as 'pgtokdb', 'getset' language c;
select count(*) from test35('test35', 1000000);

\timing off

-- Get rid of all testing artifacts
\echo Dropping test schema: pgtokdb_test
drop schema pgtokdb_test cascade;
set search_path to public;
set client_min_messages = 'notice';



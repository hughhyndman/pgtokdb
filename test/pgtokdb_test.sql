\echo Creating test schema: pgtokdb_test
SET client_min_messages = 'ERROR';

-- Create all test artifacts inside a temporary schema. It is easy to clean up
drop schema if exists pgtokdb_test cascade;
create schema pgtokdb_test;
set search_path to pgtokdb_test;

\echo Test1: Simple connectivity 
create type test1_t as (j bigint);
create function test1(varchar, int) returns setof test1_t as 'pgtokdb', 'getset' language c;
select * from test1('test1', 5);

\echo Test2: All types returned
create type test2_t as (b boolean, h smallint, i integer, j bigint, e real, 
	f double precision, p timestamp, c char, cc varchar);
create function test2(varchar, int) returns setof test2_t as 'pgtokdb', 'getset' language c;
select * from test2('test2', 5);

\echo Test3: Casting up kdb+ c, h, i, and j to bigint
create type test3_t as (j1 bigint, j2 bigint, j3 bigint, j4 bigint);
create function test3(varchar, int) returns setof test3_t as 'pgtokdb', 'getset' language c;
select * from test3('test3', 5);

\echo Test4: All supported types passed as arguments
create type test4_t as (res boolean);
create function test4(varchar, boolean, smallint, integer, bigint, real, 
	double precision, timestamp, varchar) returns setof test4_t as 'pgtokdb', 'getset' language c;
select * from test4('test4', true, cast(1 as smallint), 2, cast(3 as bigint), 
	cast(4.5 as float4), cast(6.7 as float8), cast(now() as timestamp), 
	'a varchar string');

\echo Test5: char(1) passed as argument and returned as result
create type test5_t as (c char);
create function test5(varchar, char) returns setof test5_t as 'pgtokdb', 'getset' language c;
select * from test5('test5', 'T');

\echo Test6: timestamp with/without timezone support
create type test6_t as (p1 timestamp, p2 timestamptz);
create function test6(varchar, timestamp, timestamptz) returns setof test6_t as 'pgtokdb', 'getset' language c;
select * from test6('test6', cast(now() as timestamp), now());

\echo Test7: date type support
create type test7_t as (d date);
create function test7(varchar, date) returns setof test7_t as 'pgtokdb', 'getset' language c;
select * from test7('test7', '2019-09-01');

\echo Test8: UUID (g) type support
create type test8_t as (g UUID); 
create function test8(varchar, UUID) returns setof test8_t as 'pgtokdb', 'getset' language c;
select * from test8('test8', 'A0EEBC99-9C0B-4EF8-BB6D-6BB9BD380A11');

\echo Test9: text (C) type support
create type test9_t as (tt text); 
create function test9(varchar, text) returns setof test9_t as 'pgtokdb', 'getset' language c;
select * from test9('test9', 'Here is some text');

\echo Test10: bytea (X) type support
create type test10_t as (xx bytea); 
create function test10(varchar, bytea) returns setof test10_t as 'pgtokdb', 'getset' language c;
select * from test10('test10', '\xBAADF00D');

-- Get rid of all testing artifacts
\echo Dropping test schema: pgtokdb_test
drop schema pgtokdb_test cascade;
set search_path to public;
set client_min_messages = 'notice';




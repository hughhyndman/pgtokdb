-- 
-- test1: Simple connectivity 
--
create or replace function run_test1() returns boolean as 
$$ 
begin
	drop function if exists test1;
	drop type if exists _test1;
	create type _test1 as (j bigint);
	create function test1(varchar, int) returns setof _test1 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test1('test1', 5);
	return true;
end;
$$ language plpgsql;


--
-- test2: All types returned
--
create or replace function run_test2() returns boolean as 
$$ 
begin
	drop function if exists test2;
	drop type if exists _test2;
	create type _test2 as (b boolean, h smallint, i integer, j bigint, e real, 
		f double precision, p timestamp, c char, cc varchar);
	create function test2(varchar, int) returns setof _test2 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test2('test2', 5);
	return true;
end;
$$ language plpgsql;


--
-- test3: Casting up kdb+ c, h, i, and j to bigint
--
create or replace function run_test3() returns boolean as 
$$ 
begin
	drop function if exists test3;
	drop type if exists _test3;
	create type _test3 as (j1 bigint, j2 bigint, j3 bigint, j4 bigint);
	create function test3(varchar, int) returns setof _test3 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test3('test3', 5);
	return true;
end;
$$ language plpgsql;


--
-- test4: All types passed as arguments
--
create or replace function run_test4() returns boolean as 
$$ 
begin
	drop function if exists test4;
	drop type if exists _test4;
	create type _test4 as (res boolean);
	create function test4(varchar, boolean, smallint, integer, bigint, real, 
		double precision, timestamp, varchar) returns setof _test4 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test4('test4', true, cast(1 as smallint), 2, cast(3 as bigint), 
		cast(4.5 as float4), cast(6.7 as float8), cast(now() as timestamp), 
		'a varchar string');
	return true;
end;
$$ language plpgsql;

--
-- test5: char(1) passed as argument and returned as result
--
create or replace function run_test5() returns boolean as 
$$ 
begin
	drop function if exists test5;
	drop type if exists _test5;
	create type _test5 as (c char);
	create function test5(varchar, char) returns setof _test5 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test5('test5', 'T');
	return true;
end;
$$ language plpgsql;

--
-- test6: timestamp with/without timezone support
--
create or replace function run_test6() returns boolean as 
$$ 
begin
	drop function if exists test6;
	drop type if exists _test6;
	create type _test6 as (p1 timestamp, p2 timestamptz);
	create function test6(varchar, timestamp, timestamptz) returns setof _test6 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test6('test6', cast(now() as timestamp), now());
	return true;
end;
$$ language plpgsql;

--
-- test7: date type support
--
create or replace function run_test7() returns boolean as 
$$ 
begin
	drop function if exists test7;
	drop type if exists _test7;
	create type _test7 as (d date);
	create function test7(varchar, date) returns setof _test7 as
		'pgtokdb', 'pgtokdb' language c immutable strict;
	perform * from test7('test7', '2019-09-01');
	return true;
end;
$$ language plpgsql;


create type _fun as (id bigint, vals float8, ts timestamp, str varchar);


-- select run_test1();
-- select run_test2();
-- select run_test3();
-- select run_test4();
-- select run_test5();


-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pgtokdb" to load this file. \quit

create or replace function test(varchar) returns table(i integer, j bigint)
	as '/usr/local/pgsql/lib/pgtokdb', 'kdb_query' language c immutable strict;

select * from test('([] i:1 2 3i; j:100 200 300)');
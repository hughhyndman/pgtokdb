\echo Use "CREATE EXTENSION pgtokdb;" to load this file. \quit

create schema if not exists pgtokdb;

drop type if exists pgtokdb._genddl cascade;

create type pgtokdb._genddl as (script varchar);

create function pgtokdb.genddl(varchar, varchar, varchar, varchar) 
	returns setof pgtokdb._genddl 
	as 'pgtokdb', 'pgtokdb' language c immutable strict; 
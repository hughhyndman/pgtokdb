\echo Use "CREATE EXTENSION pgtokdb;" to load this file. \quit

create schema if not exists pgtokdb;

drop type if exists pgtokdb._genddl cascade;

create type pgtokdb.genddl_t as (script varchar);

create function pgtokdb.genddl(varchar, varchar, varchar, varchar) 
	returns setof pgtokdb.genddl_t as 'pgtokdb', 'getset' language c; 

--
-- Create a simple function that is used to do a quick end-to-end test to
-- ensure that the extension is installed correctly and it can connect to
-- a kdb+ instance.
--
drop type if exists pgtokdb.getstatus_t cascade;

create type pgtokdb.getstatus_t as 
	(os varchar, version double precision, release date, timenow timestamp);

create function pgtokdb.getstatus(varchar) returns setof pgtokdb.getstatus_t 
	as 'pgtokdb', 'getset' language c;

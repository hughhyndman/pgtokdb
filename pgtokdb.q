\d .pg 

//
// Mapping between kdb+ types and Postgres types
//
KPTM:(!/) flip 0N 2#(
	"b";	"boolean";
	"h";	"smallint";
	"i";	"integer";
	"j";	"bigint";
	"e";	"float4";
	"f";	"float8";
	"c";	"varchar";
	"C";	"varchar";
	"g";	"uuid";
	"X";	"bytea";
	"d";	"date";
	"p";	"timestamp"
	);

//
// Some Postgres DDL command templates to manage types and functions
//
DTT:"drop type if exists _%f;";
DFT:"drop function if exists %f;";
CTT:"create type _%f as (%c);";
CFT:"create function %f(%a) returns setof _%f as 'pgtokdb','pgtokdb' language c immutable strict;";

//
// @desc Returns a table with Postgres scripts (DDL) 
//
// @param fnname 	{string}	- name of Postgres function to be created
// @param argtypes	{string}	- Kdb+ datatypes of arguments
// @param tblmeta	{table}		- result of meta call on kdb+ result table
//
// @returns a table that contains one Postgres command (DDL) per row. 
//
// @example
// 
// q) qfn:{[n] ([] id:til n;val:n?9999.99;ts:2019.10.01D0+1D*til n)}
// q) .pg.genddl["call_qfn";"i";meta qfn[1]]
// script
// -----------------------------------------------------------------------------------------------------------------------
// "drop function if exists call_qfn;"
// "drop type if exists call_qfn;"
// "create type _call_qfn as (id bigint, val float8, ts timestamp);"
// "create function call_qfn(varchar, integer) returns setof _call_qfn as 'pgtokdb','pgtokdb' language c immutable strict;""
// 
// The rows of this script can be written to a text file and be executed by psql
//
genddl:{[fnname;argtypes;tblmeta]
	argtypes:"C",argtypes; / Always prepend the C (varchar) for the kdb+ function
	a:2_raze ", ",/: KPTM[count[argtypes]#argtypes]; 
	c:2_raze ", ",/:string[key[tblmeta]`c],'" ",/:KPTM[value[tblmeta]`t]; 
	script:();
	script,:enlist ssr[DFT;"%f";fnname];
	script,:enlist ssr[DTT;"_%f";fnname];
	script,:enlist ssr/[CTT;2#v:"%",/:"fca";2#r:(fnname;c;a)];
	script,:enlist ssr/[CFT;v;r];
	([] script)
	}

//
// @desc This is equivalent to <gendll> except it is callable by Postgres
//
// @param fnname 	{string}	- name of Postgres function to be created
// @param argtypes	{string}	- Kdb+ datatypes of arguments
// @param tblmetaexpr {string}	- Executable string that returns the meta
//
// @example .pg.genddle["call_qfn";"i";"meta qfn[1]"]
//
// pg# create type _genddl as (script varchar); 
// pg# create function genddl(varchar, varchar, varchar, varchar) returns setof _genddl as 'pgtokdb', 'pgtokdb' language c immutable strict; 
// pg# select * from genddl('.pg.genddle', call_qfn','i','meta qfn[1]');
//                                                          script                                                         
// ------------------------------------------------------------------------------------------------------------------------
// drop function if exists call_qfn;
// drop type if exists call_qfn;
// create type _call_qfn as (id bigint, val float8, ts timestamp);
// create function call_qfn(varchar, integer) returns setof _call_qfn as 'pgtokdb','pgtokdb' language c immutable strict;
//
// or generate a script file that can be execute by psql
// pg# copy (select * from genddl('.pg.genddle', 'call_qfn', 'i', 'meta qfn[1]')) to '/tmp/f.sql';
//
genddle:{[fnname;argtypes;tblmetaexpr]
	genddl[fnname;argtypes;value tblmetaexpr]
	}

\d .

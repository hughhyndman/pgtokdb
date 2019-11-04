\d .pgtokdb

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
	"p";	"timestamp";
	"s";	"varchar"
	);

//
// Some Postgres DDL command templates to manage types and functions
//
DTT:"drop type if exists %f_t;";
DFT:"drop function if exists %f;";
CTT:"create type %f_t as (%c);";
CFT:"create function %f(%a) returns setof %f_t as 'pgtokdb','getset' language c;";

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
// "drop type if exists call_qfn_t;"
// "create type call_qfn_t as (id bigint, val float8, ts timestamp);"
// "create function call_qfn(varchar, integer) returns setof call_qfn_t as 'pgtokdb','getset' language c;""
// 
// The rows of this script can be written to a text file and be executed by psql
//
genddl:{[fnname;argtypes;tblmeta]
	argtypes:"C",argtypes; / Always prepend the C (varchar) for the kdb+ function
	a:2_raze ", ",/: KPTM[count[argtypes]#argtypes]; 
	c:2_raze ", ",/:string[key[tblmeta]`c],'" ",/:KPTM[value[tblmeta]`t]; 
	script:();
	script,:enlist ssr[DFT;"%f";fnname];
	script,:enlist ssr[DTT;"%f";fnname];
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
// @example
//
// pg# select * from pgtokdb.genddl('.pg.genddle', 'qfn','i','meta qfn[1]');
//                                                          script                                                         
// ------------------------------------------------------------------------------------------------------------------------
// drop function if exists call_qfn;
// drop type if exists call_qfn_t;
// create type call_qfn_t as (id bigint, val float8, ts timestamp);
// create function call_qfn(varchar, integer) returns setof call_qfn_t as 'pgtokdb','getset' language c;
//
// or generate a script file that can be execute by psql
// pg# copy (select * from genddl('.pg.genddle', 'qfn', 'i', 'meta qfn[1]')) to '/tmp/f.sql';
//
genddle:{[fnname;argtypes;tblmetaexpr]
	genddl[fnname;argtypes;value tblmetaexpr]
	}

\d . 

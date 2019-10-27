KPTM:(!/) flip 0N 2#(
	"b";	"boolean";
	"h";	"smallint";
	"i";	"integer";
	"j";	"bigint";
	"e";	"float4";
	"f";	"float8";
	"c";	"varchar";
	"C";	"varchar";
	"p";	"timestamp"
	);

CTT:"create type _%f as (%c);";
CFT:"create function %f(%a) returns setof _%f as 'pgtokdb','pgtokdb' language c immutable strict;";

// resmeta is the result of a meta call
bcf:{[fnname;argtypes;resmeta]
	a:2_raze ", ",/: KPTM[count[argtypes]#argtypes];
	c:2_raze ", ",/:string[key[resmeta]`c],'" ",/:KPTM[value[resmeta]`t];
	script:enlist ssr/[CTT;2#v:"%",/:"fca";2#r:(fnname;c;a)];
	script,:enlist ssr/[CFT;v;r];
	([] script)
	};

/

from kdb: `:somefile.sql 0: bcf[....][;`script];
from command line: psql -f somefile.sql

or from within postgres:

(not right -- just thinking about it)
copy (select * from bcf('bcf["foo";"i";meta([] j:1 2 3)]')) to "..." (format text);

\
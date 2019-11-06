# PostgreSQL to kdb+ Extension
This particular project is intended to integrate data in PostgreSQL with [kdb+](https://en.wikipedia.org/wiki/Kdb%2B) data. While Postgres has excellent transactional support for reference/master data, kdb+ offers a high-performance solution to storing and analyzing extreme volumes of timeseries data. By allowing a developer to combined the data from both technologies through the standard interfaces that Postgres offers, this extension expedites the development of new solutions.

With the pgtokdb extension (shared library or DLL) installed, the following is a gist of how it works. The extension has an entry point (a C function) named `getset` (a SRF: Set Returning Function), that handles communications between SQL and kdb+.

First, we create a Postgres function that wraps `getset`. This particular function takes a q-language expression that returns a simple table of two columns: i and j, 4-byte and 8-byte integers respectively.

```sql
create or replace function callkdb(varchar) returns table(i integer, j bigint) as 'pgtokdb', 'getset' language c;
```

We have a tiny q function defined that returns a simple table. A kdb+ session is running listening on a configured port waiting for work.

```q
q)qfn[]
i j  
-----
1 100
2 200
3 300
```

Let's call that function from within Postgres. 

```sql
select * from callkdb('qfn[]');
 i |  j  
---+-----
 1 | 100
 2 | 200
 3 | 300
```

Below, we have a simple Postgres table that contains some code-description values, where the i column represent the code of, say, some piece of machinery.

```sql
select * from code;
 i |     c      
---+------------
 1 | Compressor
 2 | Pump
 3 | Extruder
```

Let's join the two table inside of Postgres. 

```sql
select B.i, A.c, B.j from code A, callkdb('qfn[]') B where A.i = B.i;
 i |     c      |  j  
---+------------+-----
 1 | Compressor | 100
 2 | Pump       | 200
 3 | Extruder   | 300
```
 
Here is a bit more complex example. We want to invoke a kdb+ function that has an integer argument (`numrows`), and returns a table with a different column types. 

```q
fun:{[numrows]
    / Generate unkeyed table
    ([]
        id:numrows?1000; / Random bigints (j)
        vals:numrows?999.9; / Random floats (f)
        ts:.z.p+1000000000*til numrows; / Array of timestamps (p), starting at now 
        str:"string" ,/: string til numrows / Just some strings (C)
        )
   }
```

In Postgres, we create a type that maps to the names and data types of the kdb+ function result. Also, we create a function that connects everything. Finally, we make the call from within psql.

```sql
create type callfun_t as (id bigint, vals float8, ts timestamp, str varchar);

create function callfun(varchar, integer) returns setof callfun_t as 'pgtokdb', 'getset' language c;

select * from callfun('fun', 10);

 id  |       vals       |            ts             |   str   
-----+------------------+---------------------------+---------
 468 |  340.68741165963 | 2019-10-29 14:25:47.51818 | string0
 708 | 53.9956231920747 | 2019-10-29 14:25:48.51818 | string1
 838 | 549.738582797139 | 2019-10-29 14:25:49.51818 | string2
 682 | 195.827127312706 | 2019-10-29 14:25:50.51818 | string3
 201 | 561.469921466801 | 2019-10-29 14:25:51.51818 | string4
 458 | 70.4310706491116 | 2019-10-29 14:25:52.51818 | string5
 274 | 212.379495747993 | 2019-10-29 14:25:53.51818 | string6
 549 |  777.80420385031 | 2019-10-29 14:25:54.51818 | string7
 461 | 484.424253823073 | 2019-10-29 14:25:55.51818 | string8
 924 | 682.731623685546 | 2019-10-29 14:25:56.51818 | string9
```

## Data Types and Conversions
The table below summarizes the how the data types should be mapped between kdb+ and Postgres. The Code specifies the single character kdb+ datatype codes, which are used in the `genddl` functions in the `pgtokdb` namespace provided in the provided `pgtokdb.q` script file.

kdb+ | Code | Postgres
:-- | :-: | :-- 
boolean | b | boolean 
short | h | smallint (int2) 
int | i | int (integer, int4)
long | j | bigint (int8)
real | e | real (float4) 
float | f | double precision (float8)
GUID | g | UUID
date | d | date
timestamp | d | timestamp
char | c | varchar 
char[] | C | varchar
byte[] | X | bytea 
symbol | s | varchar 

The extension does support up-casting to data types where there won't be any data loss, for example kdb+ short to Postgres bigint. However there could be precision loss when casting integers to floats.

## Utilities
Writing wrapper Postgres function and types to specific kdb+ queries is cumbersome, so convenenient utility functions (both kdb+ and Postgres) are provided with the installation.

The kdb+ utilities (`genddl` and `genddle`) are found in the installations `pgtokdb.q` script and are placed in the `.pgtokdb` namespace. 

The example below uses a function (`qfn`) that takes an integer argument, and returns a kdb+ table with 3 columns: a long (j), float (f), and timestamp (p). We want to build the necessary Postgres function that can call this function.

```
q) qfn:{[n] ([] id:til n; val:n?9999.99; ts:2019.10.01D0+1D*til n)}
```
The `genddl` function (Generate Data Definition Language), takes three arguments: the name of the Postgres function to be created, the kdb+ data type codes of the kdb+ functon arguments, and the meta of the function result.

```
q) ddl:.pgtokdb.genddl["call_qfn"; "i"; meta qfn[1]]
q) ddl
script
-----------------------------------------------------------------------------------------------------------------------
"drop function if exists call_qfn;"
"drop type if exists _call_qfn_t;"
"create type call_qfn_t as (id bigint, val float8, ts timestamp);"
"create function call_qfn(varchar, integer) returns setof _call_qfn_t as 'pgtokdb','getset' language c;"
```
The result is a table containing the necessary DDL scripts. The table's contents can be stored in a text file for execution using psql. Note that the first argument to `call_qfn` above is the kdb+ function (or script) to be invoked.

```
q) `:/tmp/gen.sql 0: ddl[;`script]
`:/tmp/gen.sql
```

```
postgres=# \i /tmp/gen.sql
```

We can invoke a variant of `genddl` (i.e., `genddle`) from within a psql session by using the `pgtokdb.gendll` Postgres function. The difference is that the (string) expression that generates the meta is provided.

```
postgres=# select * from pgtokdb.genddl('.pgtokdb.genddle', 'call_qfn','i','metaqfn[1]');
script                                                         
-----------------------------------------------------------------------------------------------------
drop function if exists call_qfn;
drop type if exists call_qfn_t;
create type call_qfn_t as (id bigint, val float8, ts timestamp);
create function call_qfn(varchar, integer) returns setof call_qfn_t as 'pgtokdb','getset' language c;
```
One can write this to a text file for execution as follows.

```
postgres=# copy (select * from pgtokdb.genddl(...)) to '/tmp/f.sql';
```

## TODO List
* flesh out conversion variants
* documention
* move de/allocation of dvalues and nulls
* change entry point name (remove strict and immutable)
* nmake needs PGROOT passed in
* lots more

## Installation and Configuration
TODO:
* postgres.config
* c.dll (in Windows)
* target directories in each OS
* CREATE EXTENSION
* Smoke test

```q
q) select * from pgtokdb.getstatus('.pgtokdb.status[]');
 os  | version |  release   |          timenow           
-----+---------+------------+----------------------------
 m64 |     3.6 | 2018-11-09 | 2019-11-05 00:05:30.281957
```

## Sample Usage
tbd

## Build
tbd (Mac, Linux, and Windows)

### Regression Tests
The project has a test folder that contains a lengthy PGSQL script (and matching kdb+ script) that runs through both happy and exception paths of the extension. To run these tests, first start a local instance of kdb+ that loads its script file and listens on port 5000.

```
$ q pgtokdb/test/pgtokdb_test.q -p 5000
"Ready to run tests."
q) 
```

Then start psql and invoke its test script.

```
$ psql --quiet --file pgtokdb/test/pgtokdb_test.sql
Creating test schema: pgtokdb_test
************** Happy Path Testing **************
** Test01: Simple connectivity
 j 
---
 0
(1 row)
...
************** Exception Path Testing **************
** Testxx: Error on kdb+ not returning unkeyed table
psql:dev/pgtokdb/test/pgtokdb_test.sql:xx: ERROR:  Result from kdb+ must be unkeyed table
psql:dev/pgtokdb/test/pgtokdb_test.sql:xx: ERROR:  Result from kdb+ must be unkeyed table
** Testxx: Unsupported argument types
psql:dev/pgtokdb/test/pgtokdb_test.sql:xx: ERROR:  Argument 1 uses an unsupport type
```

The Happy Path Testing should not produce any errors, while the Exception Path Testing should only produce those errors that are emited from the extension.

## Acknowledgements
Aside from the excellent documentation on the Postgres site, there is a lot of good material written by software engineers on various technical aspects of writing Postgres extensions, as follows.

[Writing PostgreSQL Extensions is Fun – C Language](https://www.percona.com/blog/2019/04/05/writing-postgresql-extensions-is-fun-c-language/)

[Set Returning Functions and PostgreSQL 10](https://tapoueh.org/blog/2017/10/set-returning-functions-and-postgresql-10/)

[Introduction to MemoryContexts](https://blog.pgaddict.com/posts/introduction-to-memory-contexts)

[Project of UDF and its realization at C for PostgreSQL](https://postgres.cz/wiki/Project_of_UDF_and_its_realization_at_C_for_PostgreSQL)

[Porting a PostgreSQL Extension from Unix to Windows 10](http://www.myrkraverk.com/blog/2019/08/porting-a-postgresql-extension-from-unix-to-windows-10/)

[Writing Postgres Extensions Code Organization and Versioning](http://big-elephants.com/2015-11/writing-postgres-extensions-part-v/)




 

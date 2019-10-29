# PostgreSQL to kdb+ Extension
This particular project is intended to integrate data in PostgreSQL with [kdb+](https://en.wikipedia.org/wiki/Kdb%2B) data. While Postgres has excellent transactional support for reference/master data, kdb+ offers a high-performance solution to storing and analyzing large volumes of timeseries data. By allowing a developer to combined the data from both technologies through the standard interfaces that Postgres offers, this extension may be able to expedite the development of new solutions.

With the pgtokdb extension (SO) installed, the following is a gist of how it works. The extension has an entry point (C function) named `pgtokdb`, that handles communications between SQL and kdb+.

First, we create a Postgres function that wraps `kdb_query`. This particular function take a q-language expression that returns a simple table of two columns: i and j, 4-byte and 8-byte integers respectively.

```sql
create or replace function callkdb(varchar) returns table(i integer, j bigint) 
    as 'pgtokdb', 'pgtokdb' language c immutable strict;
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
        str:"string",/: string til numrows / Just some strings (C)
        )
   }
```

In Postgres, we create a type that maps to the names and data types of the kdb+ function result. Also, we create a function that connects everything. Finally, we make the call from within psql.

```sql
create type _fun as (id bigint, vals float8, ts timestamp, str varchar);

create function callfun(varchar, integer) returns setof _fun as
    as 'pgtokdb', 'pgtokdb' language c immutable strict;

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

Over the next little while, I'll complete this extension, but provide a more complete description on installation, configuration, and data conversion. This currently only works on my Mac, and I will be testing the build and execution on Windows and Linux platforms.

 

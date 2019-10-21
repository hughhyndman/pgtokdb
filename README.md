This particular project is intended to integrate data in PostgreSQL with kdb+ data. While Postgres as excellent transactional support for reference/master data, kdb+ offers a high-performance solution to storing and analyzing large volumes of timeseries data. By allowing a developer to combined the data from both technologies through the standard interfaces that Postgres offers, this extension may be able to expedite the development of new solutions.

With the pgtokdb extension (SO) installed, the following is a gist of how it works. The extension has an entry point named kdb_query, that handles communications between SQL and kdb+.

First, we create a Postgres function that wraps kdb_query. This particular function take a q-language expression that returns a simple table of two columns: i and j, 4-byte and 8-byte integers respectively.

```create or replace function callkdb(varchar) returns table(i integer, j bigint) as '/usr/local/pgsql/lib/pgtokdb', 'kdb_query' language c immutable strict;```

We have a q tiny function that returns a simple table. A kdb+ session is running listening on a configured port waiting for work.

```
q)qfn[]
i j  
-----
1 100
2 200
3 300
```

Let's call that function from within Postgres. 

```
select * from callkdb('qfn[]');
 i |  j  
---+-----
 1 | 100
 2 | 200
 3 | 300
 ```

Below, we have a simple Postgres table that contains some code-description values, where the i column represent the code of, say, some piece of machinery.

```
select * from code;
 i |     c      
---+------------
 1 | Compressor
 2 | Pump
 3 | Extruder
 ```

Let's join the two table inside of Postgres. 

```
select B.i, A.c, B.j from code A, callkdb('qfn[]') B where A.i = B.i;
 i |     c      |  j  
---+------------+-----
 1 | Compressor | 100
 2 | Pump       | 200
 3 | Extruder   | 300
 ```
 
Over the next little while, I'll complete this extension, but provide a more complete description on installation, configuration, and data conversion. This currently only works on my Mac, and I will be testing the build and execution on Windows and Linux platforms.

 

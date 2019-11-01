OS = mac

EXTENSION = pgtokdb			# the extensions name
SCRIPT = pgtokdb--0.0.1.sql	# script files to install
REGRESS = pgtokdb_test		# our test script file (without extension)

PG_CONFIG = pg_config

PKGLIBDIR = $(shell $(PG_CONFIG) --pkglibdir)
SHAREDIR = $(shell $(PG_CONFIG) --sharedir)

I1 = -I$(shell $(PG_CONFIG) --includedir-server)
I2 = -I$(shell $(PG_CONFIG) --includedir)/internal
INCLUDE = -I. $(I1) $(I2)

CFLAGS = -g -ggdb $(INCLUDE) -c 
LFLAGS = -L/usr/local/pgsql/lib -bundle -bundle_loader /usr/local/pgsql/bin/postgres

all: pgtokdb.so

pgtokdb.o : pgtokdb.c pgtokdb.h
	gcc $(CFLAGS) -o pgtokdb.o pgtokdb.c

convert.o : convert.c pgtokdb.h
	gcc $(CFLAGS) -o convert.o convert.c

pgtokdb.so: pgtokdb.o convert.o
	gcc $(LFLAGS) -o pgtokdb.so pgtokdb.o convert.o $(OS)/c.o

clean:
	rm -f pgtokdb.so pgtokdb.o convert.o

install: pgtokdb.so
	install -c -m 755 pgtokdb.so $(PKGLIBDIR)
	install -c -m 644 pgtokdb.control $(SHAREDIR)/extension
	install -c -m 644 pgtokdb--0.0.1.sql $(SHAREDIR)/extension

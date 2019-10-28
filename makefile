EXTENSION = pgtokdb			# the extensions name
DATA = pgtokdb--0.0.1.sql	# script files to install
REGRESS = pgtokdb_test		# our test script file (without extension)
# Below indicates a "big" modules with more than one source file.
# Place no spaces following the module name (bug in make install)
MODULE_big = pgtokdb
OBJS = pgtokdb.o convert.o	# object file targets
PG_CFLAGS = c.o -g			# addition flags for gcc; also include the Kx c.o

# Include Postgres make code
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# Ensure changes to the header file forces a build
convert.o : convert.c pgtokdb.h

pgtokdb.o : pgtokdb.c pgtokdb.h

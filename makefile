EXTENSION = pgtokdb        # the extensions name
DATA = pgtokdb--0.0.1.sql  # script files to install
REGRESS = pgtokdb_test     # our test script file (without extension)
MODULES = pgtokdb          # our c module file to build
PG_CPPFLAGS = -g -O0 
PG_CFLAGS = c.o -g 

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
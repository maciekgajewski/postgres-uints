
MODULE_big = uints
OBJS = uints_io.o uint.o uint8.o uints_numutils.o

EXTENSION = uints
DATA = uints--0.9.sql

REGRESS = uints

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# $PostgreSQL: pgsql/contrib/cube/Makefile,v 1.22 2008/08/29 13:02:32 petere Exp $

MODULE_big = trajectory
OBJS= trajectory.o trj_api.o

DATA_built = trajectory.sql

SHLIB_LINK += $(filter -lm, $(LIBS) ) 
SHLIB_LINK += /usr/lib/postgresql/postgis-2.1.so `xml2-config --libs`

PG_CPPFLAGS += -I../postgis-2.1.1 -I../postgis-2.1.1/postgis -I../postgis-2.1.1/liblwgeom -I../postgis-2.1.1/libpgcommon
PG_CPPFLAGS += `xml2-config --cflags`

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)  
include $(PGXS) 
else
subdir = contrib/trajectory
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif






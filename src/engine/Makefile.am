INCLUDES = -I$(top_srcdir)/src/ui $(all_includes)
METASOURCES = AUTO
noinst_HEADERS = linkchecker.h linkstatus.h linkstatus_impl.h searchmanager.h \
	searchmanager_impl.h linkfilter.h
libengine_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES =  libengine.la
libengine_la_SOURCES = linkchecker.cpp linkstatus.cpp searchmanager.cpp \
	linkfilter.cpp
libengine_la_LIBADD = $(LIB_KHTML) 

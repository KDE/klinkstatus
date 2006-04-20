INCLUDES = $(LIBXSLT_CFLAGS) $(all_includes)
METASOURCES = AUTO
libutils_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES =  libutils.la
noinst_HEADERS = mvector.h utils.h xsl.h
libutils_la_SOURCES = utils.cpp xsl.cpp
libutils_la_LIBADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)

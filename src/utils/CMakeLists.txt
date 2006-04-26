kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I/usr/include/libxml2 $(all_includes)
#METASOURCES = AUTO
#libutils_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES =  libutils.la
#noinst_HEADERS = mvector.h utils.h xsl.h
#libutils_la_SOURCES = utils.cpp xsl.cpp
#libutils_la_LIBADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)

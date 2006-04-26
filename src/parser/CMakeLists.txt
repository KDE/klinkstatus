kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#noinst_HEADERS =  htmlparser.h http.h mstring.h node.h node_impl.h url.h
#libparser_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES =  libparser.la
#libparser_la_SOURCES =  htmlparser.cpp http.cpp mstring.cpp node.cpp url.cpp

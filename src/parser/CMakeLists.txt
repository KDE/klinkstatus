kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(klsparser_LIB_SRCS htmlparser.cpp http.cpp mstring.cpp node.cpp url.cpp)

kde4_automoc(${klsparser_LIB_SRCS})

kde4_add_library(klsparser SHARED ${klsparser_LIB_SRCS})

target_link_libraries(klsparser  ${KDE4_KDECORE_LIBS} klsutils )

set_target_properties(klsparser PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} klsparser )

########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#noinst_HEADERS =  htmlparser.h http.h mstring.h node.h node_impl.h url.h
#libparser_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES =  libparser.la
#libparser_la_SOURCES =  htmlparser.cpp http.cpp mstring.cpp node.cpp url.cpp

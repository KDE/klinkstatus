kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${LIBXML2_INCLUDE_DIR} )


########### next target ###############

set(klsutils_LIB_SRCS utils.cpp xsl.cpp )
kde4_automoc(${klsutils_LIB_SRCS})
kde4_add_library(klsutils SHARED ${klsutils_LIB_SRCS})
target_link_libraries(klsutils  ${KDE4_KDECORE_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} )
set_target_properties(klsutils PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} klsutils )

########### install files ###############


#original Makefile.am contents follow:

#INCLUDES = -I/usr/include/libxml2 $(all_includes)
#METASOURCES = AUTO
#libutils_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES =  libutils.la
#noinst_HEADERS = mvector.h utils.h xsl.h
#libutils_la_SOURCES = utils.cpp xsl.cpp
#libutils_la_LIBADD = $(LIBXSLT_LIBS) $(LIBXML_LIBS)

kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(klsconfig_LIB_SRCS)

kde4_automoc(${klsconfig_LIB_SRCS})

kde4_add_kcfg_files(klsconfig_LIB_SRCS klsconfig.kcfgc )

kde4_add_library(klsconfig SHARED ${klsconfig_LIB_SRCS})

target_link_libraries(klsconfig  ${KDE4_KDECORE_LIBS} )

set_target_properties(klsconfig PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} klsconfig )

########### install files ###############

install_files( ${KCFG_INSTALL_DIR} FILES  klinkstatus.kcfg )

kde4_footer()



#original Makefile.am contents follow:

#kde_kcfg_DATA = klinkstatus.kcfg
#METASOURCES = AUTO
#libcfg_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES =  libcfg.la
#libcfg_la_SOURCES = dummy.cpp klsconfig.kcfgc
#AM_CPPFLAGS=$(all_includes)

kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


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
kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(top_builddir)/klinkstatus/src/cfg -I$(top_builddir)/klinkstatus/src $(all_includes)
#METASOURCES = AUTO
#libsettings_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES =  libsettings.la
#libsettings_la_SOURCES = configsearchdialog.ui dummy.cpp configresultsdialog.ui \
#	configidentificationdialog.cpp configidentificationdialogui.ui
#noinst_HEADERS = configidentificationdialog.h

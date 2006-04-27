kde4_header()

add_subdirectory( settings )

include_directories( ${CMAKE_SOURCE_DIR}/src ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_builddir)/klinkstatus/src/cfg \
#	-I$(top_builddir)/klinkstatus/src $(all_includes)
#METASOURCES = AUTO
#noinst_HEADERS = sessionwidget.h tabwidgetsession.h klshistorycombo.h \
#	resultview.h resultssearchbar.h
#libui_la_LDFLAGS = $(all_libraries)
#libui_la_LIBADD = $(top_builddir)/klinkstatus/src/cfg/libcfg.la
#noinst_LTLIBRARIES =  libui.la
#libui_la_SOURCES = sessionwidgetbase.ui sessionwidget.cpp tabwidgetsession.cpp \
#	klshistorycombo.cpp resultview.cpp treeview.cpp resultssearchbar.cpp documentrootdialog.cpp
#SUBDIRS = settings

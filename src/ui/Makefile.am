INCLUDES = -I$(top_builddir)/klinkstatus/src/cfg -I$(top_builddir)/klinkstatus/src $(all_includes)
METASOURCES = AUTO
noinst_HEADERS = celltooltip.h sessionwidget.h tablelinkstatus.h tabwidgetsession.h klshistorycombo.h resultview.h
libui_la_LDFLAGS = $(all_libraries)
libui_la_LIBADD = $(top_builddir)/klinkstatus/src/cfg/libcfg.la
noinst_LTLIBRARIES =  libui.la
libui_la_SOURCES = celltooltip.cpp sessionwidgetbase.ui sessionwidget.cpp tablelinkstatus.cpp tabwidgetsession.cpp klshistorycombo.cpp resultview.cpp
SUBDIRS =  settings

kde4_header()

add_subdirectory( settings )

include_directories( ${CMAKE_BINARY_DIR}/klinkstatus/src ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(klsui_LIB_SRCS 
documentrootdialog.cpp 
klshistorycombo.cpp 
resultssearchbar.cpp 
resultview.cpp 
sessionwidget.cpp
tabwidgetsession.cpp
treeview.cpp )

kde4_add_ui3_files(klsui_LIB_SRCS  sessionwidgetbase.ui )

kde4_automoc(${klsui_LIB_SRCS})

kde4_add_library(klsui SHARED ${klsui_LIB_SRCS})

target_link_libraries(klsui  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} klsconfig klsengine )

set_target_properties(klsui PROPERTIES VERSION 1.0.0 SOVERSION 1 )

install_targets(${LIB_INSTALL_DIR} klsui )


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

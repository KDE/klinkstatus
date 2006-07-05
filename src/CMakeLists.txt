
set(libutils_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/utils/utils.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/utils/xsl.cpp
)

include_directories( ${CMAKE_SOURCE_DIR}/src/cfg ${CMAKE_SOURCE_DIR}/src/ui )
set(libparser_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/htmlparser.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/http.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/mstring.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/node.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/url.cpp
)

set(libengine_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkchecker.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkfilter.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkstatus.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkstatushelper.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/searchmanager.cpp 
)

set(libui_SRCS 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/documentrootdialog.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/klshistorycombo.cpp
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/resultssearchbar.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/resultview.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/sessionwidget.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/tabwidgetsession.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/treeview.cpp
)

set(libuisettings_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configidentificationdialog.cpp 
)


include_directories( 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui 
    ${CMAKE_BINARY_DIR}/klinkstatus/src 
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIR}
    ${KDE4_INCLUDE_DIR} 
    ${QT_INCLUDES}  
)


########### next target ###############

set(klinkstatuspart_PART_SRCS 
    ${libutils_SRCS} 
    ${libparser_SRCS} 
    ${libengine_SRCS} 
    ${libuisettings_SRCS}
    ${libui_SRCS} 
    global.cpp 
    actionmanager.cpp  
    klinkstatus_part.cpp 
)

kde4_add_ui3_files(klinkstatuspart_PART_SRCS
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/sessionwidgetbase.ui
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configsearchdialog.ui
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configresultsdialog.ui
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configidentificationdialogui.ui
	)

kde4_add_kcfg_files(klinkstatuspart_PART_SRCS ${CMAKE_SOURCE_DIR}/klinkstatus/src/cfg/klsconfig.kcfgc)

kde4_automoc(${klinkstatuspart_PART_SRCS})

kde4_add_plugin(klinkstatuspart WITH_PREFIX ${klinkstatuspart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} klinkstatuspart )

target_link_libraries(klinkstatuspart  
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${KDE4_KDECORE_LIBS} 
    ${KDE4_KPARTS_LIBS} 
    ${KDE4_KIO_LIBS}
    ${KDE4_KHTML_LIBS}
)

install(TARGETS klinkstatuspart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(klinkstatus_SRCS klinkstatus.cpp main.cpp )

kde4_automoc(${klinkstatus_SRCS})

kde4_add_executable(klinkstatus ${klinkstatus_SRCS})

target_link_libraries(klinkstatus  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} )

install(TARGETS klinkstatus  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES  klinkstatus.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  klinkstatus_part.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  klinkstatus_part.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatuspart)
install( FILES  klinkstatus_shell.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatus)




#original Makefile.am contents follow:

## set the include path for X, qt and KDE
#INCLUDES = -I$(top_srcdir)/src/cfg -I$(top_srcdir)/src/ui $(all_includes)
#
## these are the headers for your project
#
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
## this Makefile creates both a KPart application and a KPart
##########################################################################
## APPLICATION SECTION
##########################################################################
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = klinkstatus
#
## the application source, library search path, and link libraries
#
#klinkstatus_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#klinkstatus_LDADD   = $(LIB_KPARTS)
#
#xdg_apps_DATA = klinkstatus.desktop
#
## this is where the shell's XML-GUI resource file goes
#shellrcdir = $(kde_datadir)/klinkstatus
#shellrc_DATA = klinkstatus_shell.rc
#
#
##########################################################################
## KPART SECTION
##########################################################################
#kde_module_LTLIBRARIES = libklinkstatuspart.la
#
## the Part's source, library search path, and link libraries
#
#libklinkstatuspart_la_LDFLAGS = -module -no-undefined $(KDE_PLUGIN) $(all_libraries)
#libklinkstatuspart_la_LIBADD = ui/settings/libsettings.la \
#	ui/libui.la engine/libengine.la \
#	parser/libparser.la utils/libutils.la $(LIB_KPARTS) $(LIB_KFILE)
#
## this is where the desktop file will go 
#partdesktopdir = $(kde_servicesdir)
#partdesktop_DATA = klinkstatus_part.desktop
#
## this is where the part's XML-GUI resource file goes
#partrcdir = $(kde_datadir)/klinkstatuspart
#partrc_DATA = klinkstatus_part.rc
#noinst_HEADERS = klinkstatus.h klinkstatus_part.h global.h actionmanager.h
#klinkstatus_SOURCES = klinkstatus.cpp main.cpp 
#libklinkstatuspart_la_SOURCES = klinkstatus_part.cpp global.cpp \
#	actionmanager.cpp
#SUBDIRS = cfg utils parser engine ui
#
#messages: rc.cpp
#	LIST=`find . -name \*.ui -o -name \*.kcfg -o -name \*.rc` ;\
#	if test -n "$$LIST"; then $(EXTRACTRC) $$LIST >> rc.cpp; fi
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#		$(XGETTEXT) $$LIST -o $(podir)/klinkstatus.pot; \
#	fi
#

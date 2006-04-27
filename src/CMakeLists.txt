kde4_header()

add_subdirectory( cfg )
add_subdirectory( utils )
add_subdirectory( parser )
add_subdirectory( engine )
add_subdirectory( ui )

include_directories( ${CMAKE_SOURCE_DIR}/src/cfg ${CMAKE_SOURCE_DIR}/src/ui ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(klinkstatus_SRCS klinkstatus.cpp main.cpp )

kde4_automoc(${klinkstatus_SRCS})

kde4_add_executable(klinkstatus ${klinkstatus_SRCS})

target_link_libraries(klinkstatus  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} )

install_targets(/bin klinkstatus )


########### next target ###############

set(klinkstatuspart_PART_SRCS klinkstatus_part.cpp global.cpp actionmanager.cpp )

kde4_automoc(${klinkstatuspart_PART_SRCS})

kde4_add_plugin(klinkstatuspart WITH_PREFIX ${klinkstatuspart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} klinkstatuspart )

target_link_libraries(klinkstatuspart  ${KDE4_KDECORE_LIBS} settings ui engine parser utils ${KDE4_KPARTS_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} klinkstatuspart )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES  klinkstatus.desktop )
install_files( ${SERVICES_INSTALL_DIR} FILES  klinkstatus_part.desktop )
install_files( ${DATA_INSTALL_DIR}/klinkstatuspart FILES  klinkstatus_part.rc )
install_files( ${DATA_INSTALL_DIR}/klinkstatus FILES  klinkstatus_shell.rc )

kde4_footer()



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

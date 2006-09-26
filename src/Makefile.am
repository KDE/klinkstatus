# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src/cfg -I./ui -I./ui/settings $(all_includes)

# these are the headers for your project


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# this Makefile creates both a KPart application and a KPart
#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = klinkstatus

# the application source, library search path, and link libraries

klinkstatus_LDFLAGS = $(KDE_RPATH) $(all_libraries)
klinkstatus_LDADD   = $(LIB_KPARTS)

xdg_apps_DATA = klinkstatus.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/klinkstatus
shellrc_DATA = klinkstatus_shell.rc


#########################################################################
# KPART SECTION
#########################################################################
kde_module_LTLIBRARIES = libklinkstatuspart.la

# the Part's source, library search path, and link libraries

libklinkstatuspart_la_LDFLAGS = -module -no-undefined $(KDE_PLUGIN) $(all_libraries)
libklinkstatuspart_la_LIBADD = ui/settings/libsettings.la \
	ui/libui.la engine/libengine.la \
	parser/libparser.la utils/libutils.la $(LIB_KPARTS) $(LIB_KFILE)

# this is where the desktop file will go 
partdesktopdir = $(kde_servicesdir)
partdesktop_DATA = klinkstatus_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir = $(kde_datadir)/klinkstatuspart
partrc_DATA = klinkstatus_part.rc
noinst_HEADERS = klinkstatus.h klinkstatus_part.h global.h actionmanager.h
klinkstatus_SOURCES = klinkstatus.cpp main.cpp 
libklinkstatuspart_la_SOURCES = klinkstatus_part.cpp global.cpp \
	actionmanager.cpp
SUBDIRS = cfg utils parser engine ui

messages: rc.cpp
	LIST=`find . -name \*.ui -o -name \*.kcfg -o -name \*.rc` ;\
	if test -n "$$LIST"; then $(EXTRACTRC) $$LIST >> rc.cpp; fi
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/klinkstatus.pot; \
	fi


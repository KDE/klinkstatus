kde4_header()

include_directories( ${CMAKE_BINARY_DIR}/klinkstatus/src ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(klsuisettings_LIB_SRCS configidentificationdialog.cpp )

kde4_add_ui3_files(klsuisettings_LIB_SRCS  configidentificationdialogui.ui configresultsdialog.ui configsearchdialog.ui )

kde4_automoc(${klsuisettings_LIB_SRCS})

kde4_add_library(klsuisettings SHARED ${klsuisettings_LIB_SRCS})

target_link_libraries(klsuisettings  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} klsconfig )

set_target_properties(klsuisettings PROPERTIES VERSION 1.0.0 SOVERSION 1 )

install_targets(${LIB_INSTALL_DIR} klsuisettings )


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

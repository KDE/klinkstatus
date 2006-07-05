
add_subdirectory( icons )
add_subdirectory( styles )




########### install files ###############

install( FILES   DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/icons)




#original Makefile.am contents follow:

#SUBDIRS = icons styles
#
#klinkstatusicondir = $(kde_datadir)/klinkstatus/icons

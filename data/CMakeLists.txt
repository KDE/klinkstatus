kde4_header()

add_subdirectory( icons )
add_subdirectory( styles )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/klinkstatus/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = icons styles
#
#klinkstatusicondir = $(kde_datadir)/klinkstatus/icons

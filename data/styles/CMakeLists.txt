kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/klinkstatus/styles FILES  results_stylesheet.xsl )

kde4_footer()



#original Makefile.am contents follow:

#styles_DATA = \
#	results_stylesheet.xsl
#
#stylesdir = $(kde_datadir)/klinkstatus/styles
#EXTRA_DIST = $(styles_DATA)
#

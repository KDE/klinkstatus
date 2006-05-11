kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/klinkstatuspart/pics FILES  304.png )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#klinkstatusicondir = $(kde_datadir)/klinkstatuspart/pics
#
#klinkstatusicon_DATA = 304.png
#KDE_ICON = bug

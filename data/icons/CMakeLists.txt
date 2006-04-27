kde4_header()

add_subdirectory( 32x32 )
add_subdirectory( 22x22 )
add_subdirectory( 16x16 )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = 32x32 22x22 16x16
#KDE_ICON = klinkstatus

kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LIBXML2_INCLUDE_DIR} )

########### next target ###############

set(klsutils_LIB_SRCS utils.cpp xsl.cpp )

kde4_automoc(${klsutils_LIB_SRCS})

kde4_add_library(klsutils SHARED ${klsutils_LIB_SRCS})

target_link_libraries(klsutils  ${KDE4_KDECORE_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} )

set_target_properties(klsutils PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} klsutils )


########### install files ###############


kde4_footer()
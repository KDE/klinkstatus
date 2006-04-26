kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

########### next target ###############

# set(klsconfig_LIB_SRCS htmlparser.cpp http.cpp mstring.cpp node.cpp url.cpp)
set(klsconfig_LIB_SRCS)

kde4_automoc(${klsconfig_LIB_SRCS})

kde4_add_kcfg_files(klsconfig_LIB_SRCS klsconfig.kcfgc )

kde4_add_library(klsconfig SHARED ${klsconfig_LIB_SRCS})

target_link_libraries(klsconfig  ${KDE4_KDECORE_LIBS} )

set_target_properties(klsconfig PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} klsconfig )


########### install files ###############


kde4_footer()
kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

########### next target ###############

set(klsengine_LIB_SRCS linkchecker.cpp linkfilter.cpp linkstatus.cpp linkstatushelper.cpp searchmanager.cpp )

kde4_automoc(${klsengine_LIB_SRCS})

kde4_add_library(klsengine SHARED ${klsengine_LIB_SRCS})

target_link_libraries(klsengine  ${KDE4_KDECORE_LIBS} )

set_target_properties(klsengine PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} klsengine )


########### install files ###############


kde4_footer()

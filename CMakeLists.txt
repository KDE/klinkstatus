project(klinkstatus)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions( ${QT_QTDBUS_DEFINITIONS} ${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(KdepimLibs REQUIRED)

macro_optional_find_package(LibTidy)
macro_log_feature(LIBTIDY_FOUND "libtidy" "HTML Tidy" "http://tidy.sourceforge.net" FALSE "" "Gives KlinkStatus the ability to tidy HTML")
macro_bool_to_01(LIBTIDY_FOUND HAVE_LIBTIDY)

add_subdirectory( data )
add_subdirectory( src )

macro_display_feature_log()


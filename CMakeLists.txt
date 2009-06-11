project(klinkstatus)

remove_definitions(-DKDE4_KDE3SUPPORT_LIBS -DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

macro_optional_find_package(LibTidy)
macro_log_feature(LIBTIDY_FOUND "libtidy" "HTML Tidy" "http://tidy.sourceforge.net" FALSE "" "Gives KlinkStatus the ability to tidy HTML")
macro_bool_to_01(LIBTIDY_FOUND HAVE_LIBTIDY)

add_subdirectory( data )
add_subdirectory( src )


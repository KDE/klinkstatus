project(klinkstatus)

remove_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

add_subdirectory( data )
add_subdirectory( src )


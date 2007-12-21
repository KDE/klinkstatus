project(klinkstatus)

remove_definitions(-DKDE4_KDE3SUPPORT_LIBS -DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

add_subdirectory( data )
add_subdirectory( src )


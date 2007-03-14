project(klinkstatus)

find_package(LibTidy REQUIRED)

add_subdirectory( data )
add_subdirectory( src )


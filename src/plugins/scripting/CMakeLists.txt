add_subdirectory(scripts)

include_directories(
    ${CMAKE_SOURCE_DIR}/klinkstatus/src 
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

# the KLinkStatus Scripting KParts::Plugin + Kross module
set(krossmoduleklinkstatus_PART_SRCS scriptingmodule.cpp scriptingpart.cpp)

kde4_add_plugin(krossmoduleklinkstatus ${krossmoduleklinkstatus_PART_SRCS})

target_link_libraries(krossmoduleklinkstatus
    ${KDE4_KDECORE_LIBS} 
    ${KDE4_KPARTS_LIBS}  
    ${KDE4_KROSSUI_LIBS} 
    klinkstatuscommon)


install(TARGETS krossmoduleklinkstatus DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krossmoduleklinkstatus.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatuspart/kpartplugins)

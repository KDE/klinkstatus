include_directories(
    ${CMAKE_SOURCE_DIR}/klinkstatus/src 
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

set(automationklinkstatus_PART_SRCS automationpart.cpp)

kde4_add_plugin(automationklinkstatus ${automationklinkstatus_PART_SRCS})

target_link_libraries(automationklinkstatus
    ${KDE4_KDECORE_LIBS} 
    ${KDE4_KPARTS_LIBS}  
    klinkstatuscommon)

install(TARGETS automationklinkstatus DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  klinkstatus_automation.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  klinkstatus_automation.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatuspart/kpartplugins)

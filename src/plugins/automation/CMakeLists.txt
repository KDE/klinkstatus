include_directories(
    ${CMAKE_SOURCE_DIR}/klinkstatus/src 
    ${CMAKE_BINARY_DIR}/klinkstatus/src
    ${KDEPIMLIBS_INCLUDE_DIR}
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

set(automationklinkstatus_PART_SRCS 
    automationpart.cpp
    automationdialog.cpp
    automationconfigpage.cpp
)

kde4_add_ui_files(automationklinkstatus_PART_SRCS
   ${CMAKE_SOURCE_DIR}/klinkstatus/src/plugins/automation/automationconfigpageui.ui
)

kde4_add_kcfg_files(automationklinkstatus_PART_SRCS ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/automationconfig.kcfgc)
kde4_add_kcfg_files(automationklinkstatus_PART_SRCS ${CMAKE_SOURCE_DIR}/klinkstatus/src/cfg/klsconfig.kcfgc)

kde4_add_plugin(automationklinkstatus ${automationklinkstatus_PART_SRCS})

target_link_libraries(automationklinkstatus
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDEPIMLIBS_MAILTRANSPORT_LIBS}
    klinkstatuscommon)

install(TARGETS automationklinkstatus DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  klinkstatus_automation.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  klinkstatus_automation.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatuspart/kpartplugins)

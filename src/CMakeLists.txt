
set(libutils_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/utils/utils.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/utils/xsl.cpp
)

include_directories( ${CMAKE_SOURCE_DIR}/src/cfg ${CMAKE_SOURCE_DIR}/src/ui )
set(libparser_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/htmlparser.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/http.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/mstring.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/node.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/parser/url.cpp
)

set(libengine_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkchecker.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkfilter.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkstatus.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/linkstatushelper.cpp 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/engine/searchmanager.cpp 
)

set(libui_SRCS 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/documentrootdialog.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/klshistorycombo.cpp
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/resultssearchbar.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/resultview.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/sessionwidget.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/tabwidgetsession.cpp 
	${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/treeview.cpp
)

set(libuisettings_SRCS 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configidentificationdialog.cpp 
)


include_directories( 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src 
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui 
    ${CMAKE_BINARY_DIR}/klinkstatus/src 
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIR}
    ${KDE4_INCLUDE_DIR} 
    ${QT_INCLUDES}  
)


########### next target ###############

set(klinkstatuspart_PART_SRCS 
    ${libutils_SRCS} 
    ${libparser_SRCS} 
    ${libengine_SRCS} 
    ${libuisettings_SRCS}
    ${libui_SRCS} 
    global.cpp 
    actionmanager.cpp  
    klinkstatus_part.cpp 
)

kde4_add_ui3_files(klinkstatuspart_PART_SRCS
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/sessionwidgetbase.ui
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configsearchdialog.ui
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configresultsdialog.ui
    ${CMAKE_SOURCE_DIR}/klinkstatus/src/ui/settings/configidentificationdialogui.ui
	)

kde4_add_kcfg_files(klinkstatuspart_PART_SRCS ${CMAKE_SOURCE_DIR}/klinkstatus/src/cfg/klsconfig.kcfgc)

kde4_automoc(${klinkstatuspart_PART_SRCS})

kde4_add_plugin(klinkstatuspart WITH_PREFIX ${klinkstatuspart_PART_SRCS})



target_link_libraries(klinkstatuspart  
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${KDE4_KDECORE_LIBS} 
    ${KDE4_KPARTS_LIBS} 
    ${KDE4_KIO_LIBS}
    ${KDE4_KHTML_LIBS}
)

install(TARGETS klinkstatuspart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(klinkstatus_SRCS klinkstatus.cpp main.cpp )

kde4_automoc(${klinkstatus_SRCS})

kde4_add_executable(klinkstatus ${klinkstatus_SRCS})

target_link_libraries(klinkstatus  ${KDE4_KPARTS_LIBS} )

install(TARGETS klinkstatus  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES  klinkstatus.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  klinkstatus_part.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  klinkstatus_part.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatuspart)
install( FILES  klinkstatus_shell.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatus)



add_subdirectory(plugins)

set(GENERIC_KLINKSTATUS_LIB_VERSION "4.0.0")
set(GENERIC_KLINKSTATUS_LIB_SOVERSION "4")

configure_file(config-tidy.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-tidy.h )

include_directories(
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/interfaces
    ${CMAKE_SOURCE_DIR}/src/interfaces/engine
    ${CMAKE_SOURCE_DIR}/src/interfaces/ui
    ${CMAKE_BINARY_DIR}/src
    ${KDEPIMLIBS_INCLUDE_DIR}
    ${KDE4_INCLUDE_DIR}
    ${KDE4_INCLUDE_DIR}/threadweaver
    ${KROSS_INCLUDES}
    ${QT_INCLUDES}
)

if(LIBTIDY_FOUND)
    include_directories(${LIBTIDY_INCLUDE_DIR})
endif(LIBTIDY_FOUND)



########### next target ###############

set(libutils_SRCS 
    ${CMAKE_SOURCE_DIR}/src/utils/utils.cpp     
    ${CMAKE_SOURCE_DIR}/src/utils/timer.cpp
)

set(libparser_SRCS
    ${CMAKE_SOURCE_DIR}/src/parser/htmlparser.cpp 
    ${CMAKE_SOURCE_DIR}/src/parser/http.cpp 
    ${CMAKE_SOURCE_DIR}/src/parser/mstring.cpp 
    ${CMAKE_SOURCE_DIR}/src/parser/node.cpp 
    ${CMAKE_SOURCE_DIR}/src/parser/url.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/robotsparser.cpp
)

set(libengine_SRCS 
    ${CMAKE_SOURCE_DIR}/src/engine/linkchecker.cpp 
    ${CMAKE_SOURCE_DIR}/src/engine/linkfilter.cpp 
    ${CMAKE_SOURCE_DIR}/src/engine/linkstatus.cpp 
    ${CMAKE_SOURCE_DIR}/src/engine/linkstatushelper.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/searchmanager.cpp 
    ${CMAKE_SOURCE_DIR}/src/engine/searchmanageragent.cpp 
    ${CMAKE_SOURCE_DIR}/src/engine/automationengine.cpp 
)

set(libinterfaces_SRC
    ${CMAKE_SOURCE_DIR}/src/interfaces/engine/isearchmanager.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/ui/iview.cpp
)

set(libui_SRCS 
    ${CMAKE_SOURCE_DIR}/src/ui/view.cpp 
    ${CMAKE_SOURCE_DIR}/src/ui/documentrootdialog.cpp 
    ${CMAKE_SOURCE_DIR}/src/ui/klshistorycombo.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/resultssearchbar.cpp 
    ${CMAKE_SOURCE_DIR}/src/ui/resultview.cpp 
    ${CMAKE_SOURCE_DIR}/src/ui/sessionwidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/tabwidgetsession.cpp  
    ${CMAKE_SOURCE_DIR}/src/ui/treeview.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/sessionstackedwidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/unreferreddocumentswidget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/httppostdialog.cpp
)

set(libuisettings_SRCS 
    ${CMAKE_SOURCE_DIR}/src/ui/settings/configidentificationdialog.cpp     
    ${CMAKE_SOURCE_DIR}/src/ui/settings/configresultsdialogimpl.cpp
)

set(libpim_SRCS 
    ${CMAKE_SOURCE_DIR}/src/pim/pimconfigdialog.cpp
    ${CMAKE_SOURCE_DIR}/src/pim/pimagent.cpp
)

set (klinkstatuscommon_LIB_SRCS
    ${libutils_SRCS}
    ${libparser_SRCS}
    ${libengine_SRCS}
    ${libui_SRCS}
    ${libuisettings_SRCS}
    ${libinterfaces_SRC}
    ${libpim_SRCS}
    klsfactory.cpp
    actionmanager.cpp
    global.cpp
    klinkstatusbasepart.cpp
)

kde4_add_ui_files(klinkstatuscommon_LIB_SRCS
    ${CMAKE_SOURCE_DIR}/src/ui/sessionwidgetbase.ui
    ${CMAKE_SOURCE_DIR}/src/ui/unreferreddocumentswidget.ui
    ${CMAKE_SOURCE_DIR}/src/ui/httppostwidgetui.ui
    ${CMAKE_SOURCE_DIR}/src/ui/settings/configsearchdialog.ui
    ${CMAKE_SOURCE_DIR}/src/ui/settings/configresultsdialog.ui
    ${CMAKE_SOURCE_DIR}/src/ui/settings/configidentificationdialogui.ui   
    ${CMAKE_SOURCE_DIR}/src/pim/identitywidgetui.ui
)

kde4_add_kcfg_files(klinkstatuscommon_LIB_SRCS ${CMAKE_SOURCE_DIR}/src/cfg/klsconfig.kcfgc)
kde4_add_kcfg_files(klinkstatuscommon_LIB_SRCS ${CMAKE_SOURCE_DIR}/src/engine/automationconfig.kcfgc)

if(LIBTIDY_FOUND)
    set(libtidy_SRCS ${CMAKE_SOURCE_DIR}/src/tidy/markupvalidator.cpp)
    set(klinkstatuscommon_LIB_SRCS ${klinkstatuscommon_LIB_SRCS} ${libtidy_SRCS})
endif(LIBTIDY_FOUND)


# D-Bus
qt4_add_dbus_adaptor(klinkstatuscommon_LIB_SRCS interfaces/engine/org.kde.kdewebdev.klinkstatus.SearchManager.xml isearchmanager.h ISearchManager isearchmanageradaptor ISearchManagerAdaptor)

qt4_add_dbus_adaptor(klinkstatuscommon_LIB_SRCS interfaces/ui/org.kde.kdewebdev.klinkstatus.View.xml iview.h IView iviewadaptor IViewAdaptor)


kde4_add_library(klinkstatuscommon SHARED ${klinkstatuscommon_LIB_SRCS})

target_link_libraries(klinkstatuscommon
    ${KDE4_KDECORE_LIBS} 
    ${KDE4_KPARTS_LIBS} 
    ${KDE4_KIO_LIBS}
    ${KDE4_KHTML_LIBS}
    ${KDE4_THREADWEAVER_LIBRARY}
    ${KDE4_KROSSUI_LIBS}
    ${KDE4_KNEWSTUFF2_LIBS}
    ${KDE4_KUTILS_LIBS}
    ${KDEPIMLIBS_MAILTRANSPORT_LIBS}
    ${KDEPIMLIBS_KPIMIDENTITIES_LIBS}
    ${KDEPIMLIBS_KMIME_LIBS}
)

if(LIBTIDY_FOUND)
    target_link_libraries(klinkstatuscommon ${LIBTIDY_LIBRARIES})
endif(LIBTIDY_FOUND)

set_target_properties(klinkstatuscommon 
    PROPERTIES VERSION ${GENERIC_KLINKSTATUS_LIB_VERSION} 
    SOVERSION ${GENERIC_KLINKSTATUS_LIB_SOVERSION} )

install(TARGETS klinkstatuscommon ${INSTALL_TARGETS_DEFAULT_ARGS})



########### next target ###############

set(klinkstatuspart_PART_SRCS
    klinkstatus_part.cpp
)

kde4_add_kcfg_files(klinkstatuspart_PART_SRCS ${CMAKE_SOURCE_DIR}/src/cfg/klsconfig.kcfgc)

kde4_add_plugin(klinkstatuspart ${klinkstatuspart_PART_SRCS})

target_link_libraries(klinkstatuspart klinkstatuscommon ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS})

install(TARGETS klinkstatuspart DESTINATION ${PLUGIN_INSTALL_DIR})



########### next target ###############

set(klinkstatus_SRCS
    klinkstatus.cpp main.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/trayicon.cpp)


kde4_add_executable(klinkstatus ${klinkstatus_SRCS})

target_link_libraries(klinkstatus ${KDE4_KPARTS_LIBS})

install(TARGETS klinkstatus ${INSTALL_TARGETS_DEFAULT_ARGS})



########### next target ###############

# set(klinkstatus_SRCS
#     mainCli.cpp
# )
# 
# 
# kde4_add_executable(klinkstatuscli ${klinkstatus_SRCS})
# 
# target_link_libraries(klinkstatuscli klinkstatuscommon)
# 
# install(TARGETS klinkstatuscli ${INSTALL_TARGETS_DEFAULT_ARGS})



########### install files ###############

install(FILES  klinkstatus.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  klinkstatus_part.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  klinkstatus_part.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatuspart)
install(FILES  klinkstatus_shell.rc  DESTINATION ${DATA_INSTALL_DIR}/klinkstatus)
install(FILES  klinkstatus.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR})

install(FILES interfaces/engine/org.kde.kdewebdev.klinkstatus.SearchManager.xml 
    DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})


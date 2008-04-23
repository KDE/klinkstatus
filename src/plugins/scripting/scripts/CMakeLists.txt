macro_optional_find_package(RUBY)

if(RUBY_FOUND)

find_program(RBUIC4 rbuic4 PATHS ${BIN_INSTALL_DIR})

if(RBUIC4)
    set(UI_STATISTICS ${CMAKE_CURRENT_SOURCE_DIR}/statisticsui.ui)

    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/statisticsui.rb COMMAND ${RBUIC4} ${UI_STATISTICS} -o ${CMAKE_CURRENT_BINARY_DIR}/statisticsui.rb COMMENT "Generating statisticsui.rb")
    
    ADD_CUSTOM_TARGET(statistics ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/statisticsui.rb)

    include_directories(${RUBY_INCLUDE_PATH})

    install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/statisticsui.rb
        statistics.rb 
        DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)
        
endif(RBUIC4)
        
endif(RUBY_FOUND)


install(FILES
    scripts.rc
#     klinkstatus_script.py
    DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)


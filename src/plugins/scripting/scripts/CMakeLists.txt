install(FILES
    scripts.rc
    DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)

macro_optional_find_package(RUBY)

if(RUBY_FOUND)
    
    include_directories(${RUBY_INCLUDE_PATH})

    install(FILES 
        examples/statistics/statisticsui.ui
        DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)
        
    install(PROGRAMS
        examples/statistics/statistics.rb 
        DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)
        
endif(RUBY_FOUND)



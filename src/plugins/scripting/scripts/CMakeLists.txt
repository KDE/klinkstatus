install(FILES
    scripts.rc
    DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)

macro_optional_find_package(RUBY)
macro_log_feature(RUBY_FOUND "Ruby" "An Interpreted object-oriented scripting language" "http://www.ruby-lang.org" FALSE "" "For KLinkStatus example ruby scripts")

if(RUBY_FOUND)
    
    include_directories(${RUBY_INCLUDE_PATH})

    install(FILES 
        examples/statistics/statisticsui.ui
        DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)
        
    install(PROGRAMS
        examples/statistics/statistics.rb 
        DESTINATION ${DATA_INSTALL_DIR}/klinkstatus/scripts)
        
endif(RUBY_FOUND)



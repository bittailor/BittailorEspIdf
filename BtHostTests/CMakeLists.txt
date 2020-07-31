cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(default_build_type "Debug")

project(BtHostTests)

file(GLOB_RECURSE COMPONENT_SOURCES 
    ../BtCmps/components/BtCore/src/*.cpp 
    ../BtCmps/components/BtCoreMocks/mock/*.cpp 
)

add_library(BtCore STATIC ${COMPONENT_SOURCES})
target_include_directories(BtCore PUBLIC
    ../BtCmps/components/BtCore/src
    ../BtCmps/components/BtCoreMocks/mock 
    ../BtCmps/components/Date
)

file(GLOB_RECURSE TEST_SOURCES 
    ../BtCmps/components/BtCore/test/*.cpp
)
list(APPEND TEST_SOURCES BtHostTestsMain.cpp)

add_executable( BtHostTests ${TEST_SOURCES} )
target_include_directories(BtHostTests PUBLIC ../BtCmps/components/Catch2)
target_link_libraries(BtHostTests BtCore)

# add_custom_target(check ALL COMMAND BtHostTests DEPENDS BtHostTests)

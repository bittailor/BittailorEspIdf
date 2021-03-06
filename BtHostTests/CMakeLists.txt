cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

# Set a default build type if none was specified
set(default_build_type "Debug")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(BtHostTests)

file(GLOB_RECURSE BT_CORE_SOURCES 
    ../BtCmps/components/BtCore/src/*.cpp 
    ../BtCmps/components/BtCoreMocks/mock/*.cpp 
)

add_library(BtCore STATIC ${BT_CORE_SOURCES})
target_include_directories(BtCore PUBLIC
    ../BtCmps/components/BtCore/src
    ../BtCmps/components/BtCoreMocks/mock 
    ../BtCmps/components/Date
)

file(GLOB_RECURSE BT_ALARM_CLOCK_SOURCES 
    ../BtCmps/components/BtAlarmClock/src/*.cpp 
    ../BtCmps/components/BtAlarmClockMocks/mock/*.cpp 
)

add_library(BtAlarmClock STATIC ${BT_ALARM_CLOCK_SOURCES})
target_include_directories(BtAlarmClock PUBLIC
    ../BtCmps/components/BtAlarmClock/src
    ../BtCmps/components/BtAlarmClockMocks/mock 
    ../BtCmps/components/ArduinoJson
)
target_link_libraries(BtAlarmClock BtCore)

file(GLOB_RECURSE TEST_SOURCES 
    ../BtCmps/components/BtCore/test/*.cpp
    ../BtCmps/components/BtAlarmClock/test/*.cpp
)
list(APPEND TEST_SOURCES BtHostTestsMain.cpp)

add_executable( BtHostTests ${TEST_SOURCES} )
target_include_directories(BtHostTests PUBLIC ../BtCmps/components/Catch2)
target_include_directories(BtHostTests PUBLIC ../BtCmps/components/Trompeloeil)
target_link_libraries(BtHostTests BtCore)
target_link_libraries(BtHostTests BtAlarmClock)


# add_custom_target(check ALL COMMAND BtHostTests DEPENDS BtHostTests)

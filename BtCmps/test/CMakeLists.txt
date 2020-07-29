# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

set(EXTRA_COMPONENT_DIRS "../components")

set(TEST_COMPONENTS "BtCore" "BtAlarmClock" CACHE STRING "List of components to test")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(BtCmps-UnitTestRunner)

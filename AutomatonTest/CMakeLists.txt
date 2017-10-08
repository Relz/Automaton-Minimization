PROJECT(AutomatonTest)

set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

ADD_SUBDIRECTORY(googletest-lib/googletest)
ADD_SUBDIRECTORY(tests)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
if(CMAKE_COMPILER_IS_GNUCXX)
    INCLUDE(CodeCoverage)
    SETUP_TARGET_FOR_COVERAGE(AutomatonTest_coverage AutomatonTest coverage firefox)
endif()

PROJECT(AutomatonProject)

SET(CMAKE_CXX_STANDARD 11)
ADD_DEFINITIONS(-std=c++11)

ADD_LIBRARY(PCH STATIC stdafx.cpp stdafx.h)
SET_SOURCE_FILES_PROPERTIES(AutomatonProject/stdafx.cpp
        PROPERTIES
        COMPILE_FLAGS "-include ../AutomatonProject/stdafx.h")

SET(SOURCE_FILES main.cpp constant.h)
ADD_EXECUTABLE(AutomatonProject ${SOURCE_FILES})

ADD_SUBDIRECTORY(Automaton)

TARGET_LINK_LIBRARIES(AutomatonProject PCH)
TARGET_LINK_LIBRARIES(AutomatonProject Automaton)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(AutomatonProject ${SFML_LIBRARIES})
endif()

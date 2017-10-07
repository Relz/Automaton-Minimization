INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

ADD_EXECUTABLE(AutomatonTest
        constructorTest.cpp
        )

TARGET_LINK_LIBRARIES(AutomatonTest gtest gtest_main)
TARGET_LINK_LIBRARIES(AutomatonTest Automaton)

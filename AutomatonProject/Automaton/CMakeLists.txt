SET(SOURCE_FILES
        Automaton.cpp
        Automaton.h
        ../SfmlSprite.cpp ../SfmlSprite.h)

ADD_SUBDIRECTORY(Input)
ADD_SUBDIRECTORY(Output)

ADD_LIBRARY(Automaton ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(Automaton Input)
TARGET_LINK_LIBRARIES(Automaton Output)

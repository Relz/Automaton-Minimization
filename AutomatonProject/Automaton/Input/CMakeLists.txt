PROJECT(Input)

SET(SOURCE_FILES Input.h ReadVectorMethod.h ReadLimit.h BaseSettings.h MatrixSettings.h VectorSettings.h)

ADD_LIBRARY(Input SHARED ${SOURCE_FILES})
set_target_properties(Input PROPERTIES LINKER_LANGUAGE CXX)

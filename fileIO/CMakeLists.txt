add_library(fileIO STATIC fileIO.cpp fileIO.h)

target_include_directories(fileIO PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_minimum_required(VERSION 3.18)
project(hammingOne)
enable_language(CUDA)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin )

add_subdirectory(fileIO)
add_subdirectory(dataGenerator)
add_subdirectory(hammingOneCPU)
add_subdirectory(hammingOneGPU)


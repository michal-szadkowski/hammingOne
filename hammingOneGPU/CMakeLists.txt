add_executable(hamGPU hamGPU.cu)
target_link_libraries(hamGPU PRIVATE fileIO)
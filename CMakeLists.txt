cmake_minimum_required(VERSION 3.27)
project(Pexeso C)

set(CMAKE_C_STANDARD 17)
set(CSFML_LIB_DIR "C:/pos_sem/CSFML/lib/gcc")
set(CSFML_INCLUDE_DIR "C:/pos_sem/CSFML/include")
set(CSFML_BIN_DIR "C:/pos_sem/CSFML/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/Networking/Server)
add_subdirectory(src/Networking/Client)
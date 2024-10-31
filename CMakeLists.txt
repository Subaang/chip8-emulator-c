cmake_minimum_required(VERSION 3.28)
project(Chip8_Emulator_C C)

set(CMAKE_C_STANDARD 23)

#SDL2
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "C:/SDL2-2.30.8-mingw/x86_64-w64-mingw32")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

#SDL2_Image
set(SDL2_IMAGE_PATH "C:/SDL2_image-2.8.2-mingw/x86_64-w64-mingw32")
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(Chip8_Emulator_C main.c)
target_link_libraries(Chip8_Emulator_C ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})

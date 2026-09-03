cmake_minimum_required(VERSION 3.20)
project(nes-emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(nes_core STATIC
    src/bus.cpp`
)

target_include_directories(nes_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(nes-emulator main.cpp)

target_link_libraries(nes-emulator PRIVATE nes_core)
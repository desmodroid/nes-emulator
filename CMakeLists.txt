cmake_minimum_required(VERSION 3.20)
project(nes-emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.16.0
)
FetchContent_MakeAvailable(Catch2)


add_library(nes_core STATIC
    src/bus.cpp
    src/cpu.cpp
    src/cpu_addressing_modes.cpp
    src/cpu_instructions.cpp
)

target_include_directories(nes_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(nes-emulator main.cpp)

add_executable(tests
    tests/cpu_instructions_test.cpp
)


target_link_libraries(nes-emulator PRIVATE nes_core)
target_link_libraries (tests PRIVATE nes_core Catch2::Catch2WithMain)
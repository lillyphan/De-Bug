cmake_minimum_required(VERSION 3.14)
project(TerminalGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(TerminalGame
    src/terminal.cpp
)

target_link_libraries(TerminalGame PRIVATE raylib)
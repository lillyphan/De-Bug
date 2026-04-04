cmake_minimum_required(VERSION 3.14)
project(DeBug CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(testLvl
    src/testingLevel.cpp
    src/core/LevelLoader.cpp
)

target_include_directories(testLvl PRIVATE
    src
    src/core/headers
)

target_link_libraries(testLvl PRIVATE raylib)

cmake_minimum_required(VERSION 3.14)
project(DeBug CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(main
    src/core/main.cpp
    src/core/CommandParser.cpp
    src/core/PuzzleEngine.cpp
    src/terminal/CommandPrompt.cpp
    src/terminal/Typing.cpp
    src/terminal/Dropdwn.cpp
)

target_include_directories(main PRIVATE src src/terminal raygui)
target_link_libraries(main PRIVATE raylib)

add_executable(renderTest
    src/renderTesting/renderTest.cpp
)

target_include_directories(renderTest PRIVATE src)
target_link_libraries(renderTest PRIVATE raylib)
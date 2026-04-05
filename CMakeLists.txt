cmake_minimum_required(VERSION 3.20)
project(DeBug LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)

FetchContent_MakeAvailable(raylib)

set(SOURCES
    src/core/main.cpp
    src/core/ComputerTerminal.cpp
    src/core/LevelLoader.cpp
    src/core/CommandParser.cpp
    src/core/PuzzleEngine.cpp

    src/terminal/CommandPrompt.cpp
    src/terminal/Dropdwn.cpp
    src/terminal/Typing.cpp
)

set(HEADERS
    src/core/headers/ComputerTerminal.h
    src/core/headers/LevelLoader.h
    src/core/headers/GameState.h
    src/core/headers/PuzzleLoader.h
    src/core/headers/PuzzleType.h
    src/core/headers/CommandParser.h

    src/terminal/CommandPrompt.h
    src/terminal/Dropdwn.h
    src/terminal/Typing.h
    src/terminal/PuzzleFile.h
    src/terminal/Terminal.h
)

add_executable(puzzletest src/core/oldMain.cpp)
target_include_directories(puzzletest PRIVATE src/core src/terminal)
target_link_libraries(puzzletest PRIVATE raylib)

add_executable(TerminalGame ${SOURCES} ${HEADERS})

target_include_directories(TerminalGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/terminal
    ${CMAKE_CURRENT_SOURCE_DIR}/raygui/src
)

target_link_libraries(TerminalGame PRIVATE raylib)

if (WIN32)
    target_link_libraries(TerminalGame PRIVATE
        winmm
        gdi32
        opengl32
    )
endif()
# De-Bug
epic game
Windows
To configure: cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
To build: cmake --build build
To run: .\build\TerminalGame.exe

MacOS
To remove carlos shenans: rm -rf build
To configure:  cmake -S . -B build
To build:   cmake --build build
To run:    ./build/main
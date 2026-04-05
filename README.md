# De-Bug
epic game
Windows
To get rid of old build: Remove-Item -Recurse -Force build
To configure: cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
To build: cmake --build build
To run: .\build\<file>.exe

MacOS
To remove carlos shenans: rm -rf build
To configure:  cmake -S . -B build
To build:   cmake --build build
To run:    ./build/main
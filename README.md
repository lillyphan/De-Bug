# De-Bug

![C++](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
![Library](https://img.shields.io/badge/library-raylib-green.svg)

**De-Bug** is a unique hybrid of precision platforming and logic-based puzzle solving.

```
$$$$$$$\\ $$$$$$$$\\$$$$$$$\\  $$\\  $$\\  $$$$$$\\
$$  __$$\\$$  _____|$$  __$$\\ $$ |  $$ |$$  __ $$\\
$$ |  $$ |$$ |      $$ |  $$  |$$ |  $$ |$$ /  \\__|
$$ |  $$ |$$$$$\\   $$$$$$$\\ |$$ |  $$ |$$ |  $$$$\\
$$ |  $$ |$$  __|   $$  __$$\\ $$ |  $$ |$$ |  \\_$$ |
$$ |  $$ |$$ |      $$ |  $$  |$$ |  $$ |$$ |     $$ |
$$$$$$$  |$$$$$$$$\\$$$$$$$   |\\$$$$$$  |\\$$$$$$   |
\\_______/ \\________|\\______/   \\______/  \\______/
```

### The Premise

You play as a tiny, wandering bug trapped within a interconnected computer network. To escape the system and reach the next room, you must navigate through complex digital landscapes and interact with a terminal to solve programming challenges.

Fix the code, jump the gaps, and debug your way to freedom.

---

### About the Game

We built a 3D platformer/puzzle solving game from scratch in C++ using raylib where levels are fully data-driven through a custom level loader system. Rather than hardcoding environments and challenges, the game reads structured text files to dynamically generate platforms, objects, and interactions. This makes it easy to design and iterate on levels without have to touch the core code. Furthermore, there is a focus on the command line which is implemented by a computer which is accessed by the user to complete challenges and be able to move on to the following levels.

---

### Features

- **Platforming Mechanics:** Navigate through a series of procedurally inspired digital levels, jumping between floating platforms and navigating terminal hardware.
- **Terminal-Based Puzzles:** Use an in-game terminal to solve logic and programming-based problems to unlock doors and progress.
- **Atmospheric Debugging:** Experience the high-stakes world of a system administrator trying to clean up your "mess."
- **Lightweight & Fast:** Built with C++ and `raylib` for smooth, high-performance gameplay.

---

### Controls

| Action                     | Key                           |
| :------------------------- | :---------------------------- |
| **Move**                   | `WASD`                        |
| **Jump**                   | `Space`                       |
| **Interact with Terminal** | `E` when in front of terminal |

_(Note: Customize these keys based on your actual implementation)_

---

### Installation & Building

#### **Prerequisites**

- **Windows:** [MinGW-w64](https://www.mingw-w64.org/) and [CMake](https://cmake.org/)
- **macOS:** [CMake](https://cmake.org/) and Xcode Command Line Tools
- **Library:** [raylib](https://www.raylib.com/) must be installed/available in your path.

#### **Windows (MinGW)**

To start fresh, remove the old build directory:

```powershell
Remove-Item -Recurse -Force build
```

Configure the project:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

Build the game:

```powershell
cmake --build build
```

Run the game:

```powershell
.\build\<your_executable_name>.exe
```

#### **macOS**

Configure the project:

```bash
cmake -S . -B build
```

Build the game:

```bash
cmake --build build
```

Or use the provided helper script:

```bash
./build-run-mac.sh
```

---

### Tech Stack

- **Language:** C++
- **Graphics Library:** [raylib](https://www.raylib.com/)
- **Build System:** CMake

#include "headers/LevelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

static std::string toLower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

Color parseColor(const std::string& colorName) {
    std::string c = toLower(colorName);

    if (c == "black") return BLACK;
    if (c == "white") return WHITE;
    if (c == "red") return RED;
    if (c == "green") return GREEN;
    if (c == "blue") return BLUE;
    if (c == "gray") return GRAY;
    if (c == "darkgray") return DARKGRAY;
    if (c == "brown") return BROWN;
    if (c == "yellow") return YELLOW;
    if (c == "orange") return ORANGE;
    if (c == "purple") return PURPLE;
    if (c == "pink") return PINK;
    if (c == "maroon") return MAROON;
    if (c == "lime") return LIME;
    if (c == "darkgreen") return DARKGREEN;
    if (c == "darkblue") return DARKBLUE;
    if (c == "skyblue") return SKYBLUE;
    if (c == "beige") return BEIGE;

    std::cerr << "Unknown color: " << colorName << ". Using WHITE.\n";
    return WHITE;
}

static bool parseVector3Line(const std::string& line, Vector3& outVec) {
    std::istringstream iss(line);
    return static_cast<bool>(iss >> outVec.x >> outVec.y >> outVec.z);
}

bool loadLevelFile(const std::string& filename, LevelData& level) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open level file: " << filename << '\n';
        return false;
    }

    level = LevelData{}; // clear previous level data

    std::string line;

    // Line 1: level name
    if (!std::getline(file, line)) {
        std::cerr << "Missing level name line.\n";
        return false;
    }
    level.name = line;

    // Line 2: player start position
    if (!std::getline(file, line) || !parseVector3Line(line, level.playerStart)) {
        std::cerr << "Invalid player start position line.\n";
        return false;
    }

    // Line 3: computer position
    if (!std::getline(file, line) || !parseVector3Line(line, level.computer.position)) {
        std::cerr << "Invalid computer position line.\n";
        return false;
    }
    level.computer.id = "computer";

    // Line 4: door position
    if (!std::getline(file, line) || !parseVector3Line(line, level.door.position)) {
        std::cerr << "Invalid door position line.\n";
        return false;
    }
    level.door.id = "door";

    // Remaining lines: objects
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "Box" || type == "box") {
            BoxObject box;
            std::string colorName;

            if (!(iss >> box.position.x >> box.position.y >> box.position.z
                      >> box.size.x >> box.size.y >> box.size.z
                      >> colorName)) {
                std::cerr << "Invalid Box line: " << line << '\n';
                continue;
            }

            box.color = parseColor(colorName);
            level.boxes.push_back(box);
        }
        else if (type == "Plane" || type == "plane") {
            PlaneObject plane;
            std::string colorName;

            if (!(iss >> plane.position.x >> plane.position.y >> plane.position.z
                      >> plane.size.x >> plane.size.y
                      >> colorName)) {
                std::cerr << "Invalid Plane line: " << line << '\n';
                continue;
            }

            plane.color = parseColor(colorName);
            level.planes.push_back(plane);
        }
        else {
            std::cerr << "Unknown object type: " << type << '\n';
        }
    }

    return true;
}

BoundingBox makeBoundingBox(Vector3 center, Vector3 size) {
    Vector3 half = {
        size.x / 2.0f,
        size.y / 2.0f,
        size.z / 2.0f
    };

    BoundingBox box;
    box.min = {
        center.x - half.x,
        center.y - half.y,
        center.z - half.z
    };
    box.max = {
        center.x + half.x,
        center.y + half.y,
        center.z + half.z
    };

    return box;
}

BoundingBox getComputerBounds(const LevelData& level) {
    return makeBoundingBox(level.computer.position, COMPUTER_SIZE);
}

BoundingBox getDoorBounds(const LevelData& level) {
    return makeBoundingBox(level.door.position, DOOR_SIZE);
}
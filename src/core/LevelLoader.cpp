#include "headers/LevelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// Note: Shapes parsed via LevelLoader fall back safely to standard rendering.
// This remains compatible with the lighting shader restrictions since Models handle their own shaders directly.

static std::string toLower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

static bool parseVector3Line(const std::string& line, Vector3& outVec) {
    std::istringstream iss(line);
    return static_cast<bool>(iss >> outVec.x >> outVec.y >> outVec.z);
}

Color parseColor(const std::string& colorToken) {
    std::string c = toLower(trim(colorToken));

    int r, g, b, a;
    char comma1, comma2, comma3;
    std::istringstream rgba(c);
    if ((rgba >> r >> comma1 >> g >> comma2 >> b >> comma3 >> a) &&
        comma1 == ',' && comma2 == ',' && comma3 == ',') {
        return Color{
            (unsigned char)r,
            (unsigned char)g,
            (unsigned char)b,
            (unsigned char)a
        };
    }

    if (c == "black") return BLACK;
    if (c == "white") return WHITE;
    if (c == "red") return RED;
    if (c == "green") return GREEN;
    if (c == "blue") return BLUE;
    if (c == "gray") return GRAY;
    if (c == "darkgray") return DARKGRAY;
    if (c == "brown") return BROWN;
    if (c == "darkbrown") return DARKBROWN;
    if (c == "yellow") return YELLOW;
    if (c == "orange") return ORANGE;
    if (c == "purple") return PURPLE;
    if (c == "violet") return VIOLET;
    if (c == "pink") return PINK;
    if (c == "magenta") return MAGENTA;
    if (c == "maroon") return MAROON;
    if (c == "lime") return LIME;
    if (c == "gold") return GOLD;
    if (c == "darkgreen") return DARKGREEN;
    if (c == "darkblue") return DARKBLUE;
    if (c == "skyblue") return SKYBLUE;
    if (c == "beige") return BEIGE;
    if (c == "lightgray") return LIGHTGRAY;

    std::cerr << "Unknown color: " << colorToken << ". Using WHITE.\n";
    return WHITE;
}

bool loadLevelFile(const std::string& filename, LevelData& level) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open level file: " << filename << '\n';
        return false;
    }

    level = LevelData{};
    level.computer.size = DEFAULT_COMPUTER_SIZE;
    level.door.size = DEFAULT_DOOR_SIZE;

    std::string line;

    // Line 1: level name
    if (!std::getline(file, line)) {
        std::cerr << "Missing level name line.\n";
        return false;
    }
    level.name = line;

    // Line 2: player start
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

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "Box" || type == "box") {
            BoxObject box;
            std::string colorToken;

            if (!(iss >> box.position.x >> box.position.y >> box.position.z
                      >> box.size.x >> box.size.y >> box.size.z
                      >> colorToken)) {
                std::cerr << "Invalid Box line: " << line << '\n';
                continue;
            }

            box.color = parseColor(colorToken);
            level.boxes.push_back(box);
        }
        else if (type == "Plane" || type == "plane") {
            PlaneObject plane;
            std::string colorToken;

            if (!(iss >> plane.position.x >> plane.position.y >> plane.position.z
                      >> plane.size.x >> plane.size.y
                      >> colorToken)) {
                std::cerr << "Invalid Plane line: " << line << '\n';
                continue;
            }

            plane.color = parseColor(colorToken);
            level.planes.push_back(plane);
        }
        else if (type == "Sphere" || type == "sphere") {
            SphereObject sphere;
            std::string colorToken;

            if (!(iss >> sphere.position.x >> sphere.position.y >> sphere.position.z
                      >> sphere.radius
                      >> colorToken)) {
                std::cerr << "Invalid Sphere line: " << line << '\n';
                continue;
            }

            sphere.color = parseColor(colorToken);
            level.spheres.push_back(sphere);
        }
        else if (type == "CylinderEx" || type == "cylinderex") {
            CylinderExObject cyl;
            std::string colorToken;

            if (!(iss >> cyl.startPos.x >> cyl.startPos.y >> cyl.startPos.z
                      >> cyl.endPos.x >> cyl.endPos.y >> cyl.endPos.z
                      >> cyl.startRadius >> cyl.endRadius >> cyl.sides
                      >> colorToken)) {
                std::cerr << "Invalid CylinderEx line: " << line << '\n';
                continue;
            }

            cyl.color = parseColor(colorToken);
            level.cylinders.push_back(cyl);
        }
        else if (type == "Fish" || type == "fish") {
            FishObject f;
            std::string colorToken;

            if (!(iss >> f.position.x >> f.position.y >> f.position.z
                      >> colorToken >> f.facing)) {
                std::cerr << "Invalid Fish line: " << line << '\n';
                continue;
            }

            f.color = parseColor(colorToken);
            level.fish.push_back(f);
        }
        else if (type == "Jellyfish" || type == "jellyfish") {
            JellyfishObject j;
            std::string colorToken;

            if (!(iss >> j.position.x >> j.position.y >> j.position.z
                      >> colorToken)) {
                std::cerr << "Invalid Jellyfish line: " << line << '\n';
                continue;
            }

            j.color = parseColor(colorToken);
            level.jellyfish.push_back(j);
        }
        else if (type == "Seaweed" || type == "seaweed") {
            SeaweedObject s;
            std::string colorToken;

            if (!(iss >> s.basePos.x >> s.basePos.y >> s.basePos.z
                      >> colorToken)) {
                std::cerr << "Invalid Seaweed line: " << line << '\n';
                continue;
            }

            s.color = parseColor(colorToken);
            level.seaweed.push_back(s);
        }
        else if (type == "BubbleColumn" || type == "bubblecolumn") {
            BubbleColumnObject b;

            if (!(iss >> b.basePos.x >> b.basePos.y >> b.basePos.z
                      >> b.timeOffset)) {
                std::cerr << "Invalid BubbleColumn line: " << line << '\n';
                continue;
            }

            level.bubbleColumns.push_back(b);
        }
        else if (type == "CatInBox" || type == "catinbox") {
            CatInBoxObject c;
            std::string colorToken;

            if (!(iss >> c.position.x >> c.position.y >> c.position.z
                      >> colorToken)) {
                std::cerr << "Invalid CatInBox line: " << line << '\n';
                continue;
            }

            c.catColor = parseColor(colorToken);
            level.catInBoxes.push_back(c);
        }
        else if (type == "ToriiGate" || type == "toriigate") {
            ToriiGateObject t;

            if (!(iss >> t.basePos.x >> t.basePos.y >> t.basePos.z)) {
                std::cerr << "Invalid ToriiGate line: " << line << '\n';
                continue;
            }

            level.toriiGates.push_back(t);
        }
        else if (type == "Lantern" || type == "lantern") {
            LanternObject l;

            if (!(iss >> l.basePos.x >> l.basePos.y >> l.basePos.z)) {
                std::cerr << "Invalid Lantern line: " << line << '\n';
                continue;
            }

            level.lanterns.push_back(l);
        }
        else if (type == "PetalPile" || type == "petalpile") {
            PetalPileObject p;

            if (!(iss >> p.position.x >> p.position.y >> p.position.z)) {
                std::cerr << "Invalid PetalPile line: " << line << '\n';
                continue;
            }

            level.petalPiles.push_back(p);
        }
        else if (type == "BackgroundBlossomTree" || type == "backgroundblossomtree") {
            BackgroundBlossomTreeObject t;

            if (!(iss >> t.trunkPos.x >> t.trunkPos.y >> t.trunkPos.z
                      >> t.trunkHeight)) {
                std::cerr << "Invalid BackgroundBlossomTree line: " << line << '\n';
                continue;
            }

            level.backgroundBlossomTrees.push_back(t);
        }
        else if (type == "Cloud" || type == "cloud") {
            CloudObject c;

            if (!(iss >> c.position.x >> c.position.y >> c.position.z)) {
                std::cerr << "Invalid Cloud line: " << line << '\n';
                continue;
            }

            level.clouds.push_back(c);
        }
        else if (type == "ComputerSize" || type == "computersize") {
            if (!(iss >> level.computer.size.x >> level.computer.size.y >> level.computer.size.z)) {
                std::cerr << "Invalid ComputerSize line: " << line << '\n';
                continue;
            }
        }
        else if (type == "DoorSize" || type == "doorsize") {
            if (!(iss >> level.door.size.x >> level.door.size.y >> level.door.size.z)) {
                std::cerr << "Invalid DoorSize line: " << line << '\n';
                continue;
            }
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
    return makeBoundingBox(level.computer.position, level.computer.size);
}

BoundingBox getDoorBounds(const LevelData& level) {
    return makeBoundingBox(level.door.position, level.door.size);
}

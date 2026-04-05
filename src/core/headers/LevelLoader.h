#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "raylib.h"
#include <string>
#include <vector>

struct BoxObject {
    Vector3 position;
    Vector3 size;
    Color color;
};

struct PlaneObject {
    Vector3 position;
    Vector2 size;
    Color color;
};

struct InteractableObject {
    std::string id;
    Vector3 position;
};

struct LevelData {
    std::string name;
    Vector3 playerStart;

    InteractableObject computer;
    InteractableObject door;

    std::vector<BoxObject> boxes;
    std::vector<PlaneObject> planes;
};

// Fixed dimensions for special objects
constexpr Vector3 COMPUTER_SIZE = { 2.0f, 1.5f, 1.5f };
constexpr Vector3 DOOR_SIZE     = { 8.0f, 14.0f, 2.0f };

Color parseColor(const std::string& colorName);
bool loadLevelFile(const std::string& filename, LevelData& level);

// Collision helpers
BoundingBox makeBoundingBox(Vector3 center, Vector3 size);
BoundingBox getComputerBounds(const LevelData& level);
BoundingBox getDoorBounds(const LevelData& level);

#endif
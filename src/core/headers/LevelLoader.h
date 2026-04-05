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

struct SphereObject {
    Vector3 position;
    float radius;
    Color color;
};

struct CylinderExObject {
    Vector3 startPos;
    Vector3 endPos;
    float startRadius;
    float endRadius;
    int sides;
    Color color;
};

struct FishObject {
    Vector3 position;
    Color color;
    float facing;
};

struct JellyfishObject {
    Vector3 position;
    Color color;
};

struct SeaweedObject {
    Vector3 basePos;
    Color color;
};

struct BubbleColumnObject {
    Vector3 basePos;
    float timeOffset;
};

struct CatInBoxObject {
    Vector3 position;
    Color catColor;
};

struct ToriiGateObject {
    Vector3 basePos;
};

struct LanternObject {
    Vector3 basePos;
};

struct PetalPileObject {
    Vector3 position;
};

struct BackgroundBlossomTreeObject {
    Vector3 trunkPos;
    float trunkHeight;
};

struct CloudObject {
    Vector3 position;
};

struct InteractableObject {
    std::string id;
    Vector3 position;
    Vector3 size;
};

struct LevelData {
    std::string name;
    Vector3 playerStart;

    InteractableObject computer;
    InteractableObject door;

    std::vector<BoxObject> boxes;
    std::vector<PlaneObject> planes;
    std::vector<SphereObject> spheres;
    std::vector<CylinderExObject> cylinders;
    std::vector<FishObject> fish;
    std::vector<JellyfishObject> jellyfish;
    std::vector<SeaweedObject> seaweed;
    std::vector<BubbleColumnObject> bubbleColumns;

    std::vector<CatInBoxObject> catInBoxes;
    std::vector<ToriiGateObject> toriiGates;
    std::vector<LanternObject> lanterns;
    std::vector<PetalPileObject> petalPiles;
    std::vector<BackgroundBlossomTreeObject> backgroundBlossomTrees;
    std::vector<CloudObject> clouds;
};

constexpr Vector3 DEFAULT_COMPUTER_SIZE = { 2.0f, 1.5f, 1.5f };
constexpr Vector3 DEFAULT_DOOR_SIZE     = { 8.0f, 14.0f, 2.0f };

Color parseColor(const std::string& colorToken);
bool loadLevelFile(const std::string& filename, LevelData& level);

BoundingBox makeBoundingBox(Vector3 center, Vector3 size);
BoundingBox getComputerBounds(const LevelData& level);
BoundingBox getDoorBounds(const LevelData& level);

#endif
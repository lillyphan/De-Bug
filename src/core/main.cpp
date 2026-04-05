#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "headers/LevelLoader.h"
#include "headers/ComputerTerminal.h"

using namespace std;

float PLAYER_RADIUS = 2.0f;
const float FLOOR_Y = -50.0f;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
void boundPos(Vector3 &var,
              float xMin, float xMax,
              float yMin, float yMax,
              float zMin, float zMax) {
    if (var.x > xMax) var.x = xMax;
    if (var.x < xMin) var.x = xMin;
    if (var.y > yMax) var.y = yMax;
    if (var.y < yMin) var.y = yMin;
    if (var.z > zMax) var.z = zMax;
    if (var.z < zMin) var.z = zMin;
}

BoundingBox makePlayerBox(Vector3 center) {
    return makeBoundingBox(
        center,
        { PLAYER_RADIUS * 2.0f, PLAYER_RADIUS * 2.0f, PLAYER_RADIUS * 2.0f }
    );
}

bool isRoomShellBox(const BoxObject& box) {
    return (box.size.x >= 90.0f || box.size.y >= 90.0f || box.size.z >= 90.0f);
}

bool isCollidableBox(const BoxObject& box) {
    if (isRoomShellBox(box)) return false;
    return (box.size.x >= 0.5f && box.size.y >= 0.1f && box.size.z >= 0.5f);
}

BoundingBox getSphereBounds(const SphereObject& s) {
    Vector3 size = { s.radius * 2.0f, s.radius * 2.0f, s.radius * 2.0f };
    return makeBoundingBox(s.position, size);
}

BoundingBox getCylinderExBounds(const CylinderExObject& c) {
    float radius = std::max(c.startRadius, c.endRadius);

    float minX = std::min(c.startPos.x, c.endPos.x) - radius;
    float maxX = std::max(c.startPos.x, c.endPos.x) + radius;

    float minY = std::min(c.startPos.y, c.endPos.y) - radius;
    float maxY = std::max(c.startPos.y, c.endPos.y) + radius;

    float minZ = std::min(c.startPos.z, c.endPos.z) - radius;
    float maxZ = std::max(c.startPos.z, c.endPos.z) + radius;

    BoundingBox box;
    box.min = { minX, minY, minZ };
    box.max = { maxX, maxY, maxZ };
    return box;
}

vector<BoundingBox> buildPlatformBounds(const LevelData& level) {
    vector<BoundingBox> bounds;

    for (const BoxObject& box : level.boxes) {
        if (isCollidableBox(box)) {
            bounds.push_back(makeBoundingBox(box.position, box.size));
        }
    }

    return bounds;
}

vector<BoundingBox> buildSolidBounds(const LevelData& level) {
    vector<BoundingBox> solids;

    for (const BoxObject& box : level.boxes) {
        if (isCollidableBox(box)) {
            solids.push_back(makeBoundingBox(box.position, box.size));
        }
    }

    for (const SphereObject& s : level.spheres) {
        solids.push_back(getSphereBounds(s));
    }

    for (const CylinderExObject& c : level.cylinders) {
        solids.push_back(getCylinderExBounds(c));
    }

    solids.push_back(getComputerBounds(level));
    solids.push_back(getDoorBounds(level));

    return solids;
}

bool overlapsHorizontally(const BoundingBox& a, const BoundingBox& b) {
    return (a.max.x > b.min.x && a.min.x < b.max.x &&
            a.max.z > b.min.z && a.min.z < b.max.z);
}

bool isStandingOnTopOf(const BoundingBox& playerBox, const BoundingBox& box) {
    float playerBottom = playerBox.min.y;
    float boxTop = box.max.y;

    return fabs(playerBottom - boxTop) <= 0.3f &&
           overlapsHorizontally(playerBox, box);
}

bool collidesWithSolidSideways(Vector3 pos, const vector<BoundingBox>& solidBounds) {
    BoundingBox playerBox = makePlayerBox(pos);

    for (const BoundingBox& box : solidBounds) {
        if (!CheckCollisionBoxes(playerBox, box)) continue;

        if (isStandingOnTopOf(playerBox, box)) {
            continue;
        }

        return true;
    }

    return false;
}

bool isTouchingBox(const BoundingBox& a, const BoundingBox& b, float eps = 0.25f) {
    bool overlapX = (a.max.x >= b.min.x - eps && a.min.x <= b.max.x + eps);
    bool overlapY = (a.max.y >= b.min.y - eps && a.min.y <= b.max.y + eps);
    bool overlapZ = (a.max.z >= b.min.z - eps && a.min.z <= b.max.z + eps);
    return overlapX && overlapY && overlapZ;
}

void showLevelTitle(const std::string& name) {
    double startTime = GetTime();
    while (GetTime() - startTime < 1.0) {
        BeginDrawing();
            ClearBackground(BLACK);
            int fontSize = 40;
            int textW = MeasureText(name.c_str(), fontSize);
            DrawText(name.c_str(),
                GetScreenWidth()/2  - textW/2,
                GetScreenHeight()/2 - fontSize/2,
                fontSize, GREEN);
        EndDrawing();
    }
}

// -----------------------------------------------------------------------------
// Decorative drawing
// -----------------------------------------------------------------------------
void drawFish(Vector3 pos, Color bodyColor, float facing) {
    DrawSphere(pos, 1.2f, bodyColor);
    DrawSphere({ pos.x + 0.9f * facing, pos.y + 0.15f, pos.z + 0.9f }, 0.12f, BLACK);

    rlPushMatrix();
    rlTranslatef(pos.x - 1.5f * facing, pos.y, pos.z);
    rlRotatef((facing > 0.0f) ? 25.0f : -25.0f, 0.0f, 1.0f, 0.0f);
    DrawCubeV({ 0.0f, 0.0f, 0.0f }, { 1.2f, 1.0f, 0.4f }, bodyColor);
    rlPopMatrix();

    DrawCubeV({ pos.x - 0.2f * facing, pos.y + 0.7f, pos.z }, { 0.9f, 0.25f, 0.5f }, bodyColor);
}

void drawJellyfish(Vector3 pos, Color bodyColor) {
    DrawSphere(pos, 1.3f, bodyColor);

    for (int i = -1; i <= 1; i++) {
        float x = pos.x + i * 0.5f;
        DrawCylinderEx(
            { x, pos.y - 0.6f, pos.z + 0.1f * i },
            { x + 0.2f * i, pos.y - 3.2f, pos.z + 0.15f * i },
            0.08f,
            0.05f,
            6,
            Fade(bodyColor, 0.8f)
        );
    }
}

void drawSeaweed(Vector3 basePos, Color color) {
    DrawCylinderEx(
        { basePos.x, basePos.y, basePos.z },
        { basePos.x + 0.4f, basePos.y + 4.5f, basePos.z + 0.2f },
        0.30f,
        0.18f,
        6,
        color
    );

    DrawCylinderEx(
        { basePos.x + 0.6f, basePos.y, basePos.z - 0.3f },
        { basePos.x + 1.0f, basePos.y + 3.6f, basePos.z + 0.5f },
        0.24f,
        0.14f,
        6,
        color
    );
}

void drawBubbleColumn(Vector3 basePos, float timeOffset) {
    for (int i = 0; i < 5; i++) {
        float rise = fmodf((float)GetTime() * 1.2f + timeOffset + i * 1.4f, 8.0f);
        Vector3 bubblePos = {
            basePos.x + sinf((float)GetTime() + i) * 0.15f,
            basePos.y + rise,
            basePos.z + cosf((float)GetTime() + i) * 0.15f
        };
        DrawSphere(bubblePos, 0.22f + 0.03f * i, Fade(SKYBLUE, 0.65f));
    }
}

void drawCatInBox(Vector3 boxPos, Color catColor) {
    Color boxColor = { 181, 140, 99, 255 };

    DrawCubeV(boxPos, { 4.0f, 2.0f, 4.0f }, boxColor);
    DrawCubeWiresV(boxPos, { 4.0f, 2.0f, 4.0f }, WHITE);

    Vector3 headPos = { boxPos.x, boxPos.y + 1.8f, boxPos.z };
    DrawSphere(headPos, 1.2f, catColor);

    DrawCubeV({ headPos.x - 0.78f, headPos.y + 0.82f, headPos.z - 0.10f }, { 0.72f, 0.34f, 0.72f }, catColor);
    DrawCubeV({ headPos.x - 0.78f, headPos.y + 1.12f, headPos.z - 0.10f }, { 0.50f, 0.28f, 0.50f }, catColor);
    DrawCubeV({ headPos.x - 0.78f, headPos.y + 1.38f, headPos.z - 0.10f }, { 0.26f, 0.22f, 0.26f }, catColor);

    DrawCubeV({ headPos.x + 0.78f, headPos.y + 0.82f, headPos.z - 0.10f }, { 0.72f, 0.34f, 0.72f }, catColor);
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 1.12f, headPos.z - 0.10f }, { 0.50f, 0.28f, 0.50f }, catColor);
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 1.38f, headPos.z - 0.10f }, { 0.26f, 0.22f, 0.26f }, catColor);

    DrawCubeV({ headPos.x - 0.78f, headPos.y + 1.03f, headPos.z + 0.28f }, { 0.26f, 0.26f, 0.26f }, PINK);
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 1.03f, headPos.z + 0.28f }, { 0.26f, 0.26f, 0.26f }, PINK);

    DrawSphere({ headPos.x - 0.38f, headPos.y + 0.20f, headPos.z + 1.22f }, 0.18f, BLACK);
    DrawSphere({ headPos.x + 0.38f, headPos.y + 0.20f, headPos.z + 1.22f }, 0.18f, BLACK);

    DrawSphere({ headPos.x, headPos.y - 0.08f, headPos.z + 1.26f }, 0.15f, BLACK);
}

void drawToriiGate(Vector3 basePos) {
    Color gateRed = { 170, 35, 35, 255 };
    Color darkWood = { 70, 45, 30, 255 };
    Color accentBlack = BLACK;

    DrawCubeV({ basePos.x - 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, gateRed);
    DrawCubeWiresV({ basePos.x - 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, accentBlack);

    DrawCubeV({ basePos.x + 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, gateRed);
    DrawCubeWiresV({ basePos.x + 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, accentBlack);

    DrawCubeV({ basePos.x, basePos.y + 28.0f, basePos.z }, { 34.0f, 3.0f, 3.5f }, gateRed);
    DrawCubeWiresV({ basePos.x, basePos.y + 28.0f, basePos.z }, { 34.0f, 3.0f, 3.5f }, accentBlack);

    DrawCubeV({ basePos.x, basePos.y + 34.0f, basePos.z }, { 46.0f, 3.0f, 4.5f }, gateRed);
    DrawCubeWiresV({ basePos.x, basePos.y + 34.0f, basePos.z }, { 46.0f, 3.0f, 4.5f }, accentBlack);

    DrawCubeV({ basePos.x, basePos.y + 37.0f, basePos.z }, { 38.0f, 1.4f, 2.8f }, darkWood);
    DrawCubeWiresV({ basePos.x, basePos.y + 37.0f, basePos.z }, { 38.0f, 1.4f, 2.8f }, accentBlack);

    DrawCubeV({ basePos.x, basePos.y + 22.0f, basePos.z }, { 2.3f, 12.0f, 2.3f }, darkWood);
    DrawCubeWiresV({ basePos.x, basePos.y + 22.0f, basePos.z }, { 2.3f, 12.0f, 2.3f }, accentBlack);
}

void drawLantern(Vector3 basePos) {
    Color wood = { 90, 60, 35, 255 };
    Color lanternRed = { 185, 55, 55, 255 };
    Color glow = { 255, 220, 160, 220 };

    DrawCubeV({ basePos.x, basePos.y + 2.5f, basePos.z }, { 0.8f, 5.0f, 0.8f }, wood);
    DrawCubeWiresV({ basePos.x, basePos.y + 2.5f, basePos.z }, { 0.8f, 5.0f, 0.8f }, BLACK);

    DrawCubeV({ basePos.x, basePos.y + 5.6f, basePos.z }, { 2.0f, 2.2f, 2.0f }, lanternRed);
    DrawCubeWiresV({ basePos.x, basePos.y + 5.6f, basePos.z }, { 2.0f, 2.2f, 2.0f }, BLACK);

    DrawCubeV({ basePos.x, basePos.y + 5.6f, basePos.z }, { 1.2f, 1.4f, 1.2f }, glow);
    DrawCubeV({ basePos.x, basePos.y + 7.0f, basePos.z }, { 2.4f, 0.3f, 2.4f }, wood);
}

void drawPetalPile(Vector3 pos) {
    Color petal = { 245, 185, 210, 255 };
    DrawSphere({ pos.x, pos.y, pos.z }, 0.7f, petal);
    DrawSphere({ pos.x + 0.8f, pos.y + 0.05f, pos.z + 0.3f }, 0.5f, petal);
    DrawSphere({ pos.x - 0.7f, pos.y + 0.02f, pos.z - 0.4f }, 0.45f, petal);
    DrawSphere({ pos.x + 0.3f, pos.y + 0.02f, pos.z - 0.8f }, 0.4f, petal);
}

void drawBackgroundBlossomTree(Vector3 trunkPos, float trunkHeight) {
    Color trunk = { 85, 55, 38, 255 };
    Color blossom = { 242, 170, 205, 255 };

    DrawCubeV({ trunkPos.x, trunkPos.y + trunkHeight / 2.0f, trunkPos.z }, { 3.5f, trunkHeight, 3.5f }, trunk);
    DrawCubeWiresV({ trunkPos.x, trunkPos.y + trunkHeight / 2.0f, trunkPos.z }, { 3.5f, trunkHeight, 3.5f }, BLACK);

    float topY = trunkPos.y + trunkHeight + 1.0f;

    DrawSphere({ trunkPos.x, topY + 1.0f, trunkPos.z }, 3.0f, blossom);
    DrawSphere({ trunkPos.x - 2.6f, topY, trunkPos.z }, 2.3f, blossom);
    DrawSphere({ trunkPos.x + 2.6f, topY, trunkPos.z }, 2.3f, blossom);
    DrawSphere({ trunkPos.x, topY, trunkPos.z - 2.6f }, 2.3f, blossom);
    DrawSphere({ trunkPos.x, topY, trunkPos.z + 2.6f }, 2.3f, blossom);
}

void drawCloud(Vector3 pos) {
    Color cloudColor = { 255, 250, 252, 220 };

    DrawSphere({ pos.x, pos.y, pos.z }, 2.4f, cloudColor);
    DrawSphere({ pos.x - 2.2f, pos.y - 0.2f, pos.z + 0.4f }, 1.8f, cloudColor);
    DrawSphere({ pos.x + 2.1f, pos.y - 0.1f, pos.z - 0.3f }, 1.9f, cloudColor);
    DrawSphere({ pos.x + 0.4f, pos.y + 1.0f, pos.z }, 1.7f, cloudColor);
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(void) {
    const int screenWidth = 1000;
    const int screenHeight = 700;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");
    InitAudioDevice();

    Sound walkSound     = LoadSound("src/assets/sounds/walk.mp3");
    Sound jumpSound     = LoadSound("src/assets/sounds/jump.mp3");
    Sound computerSound = LoadSound("src/assets/sounds/computer.mp3");
    Sound doorSound     = LoadSound("src/assets/sounds/door.mp3");
    Sound key           = LoadSound("src/assets/sounds/key1.mp3");

    LevelData level;
    if (!loadLevelFile("src/assets/rooms/room1.txt", level)) {
        cout << "Failed to load level file.\n";
        UnloadSound(walkSound);
        UnloadSound(jumpSound);
        UnloadSound(computerSound);
        UnloadSound(doorSound);
        UnloadSound(key);
        CloseAudioDevice();
        CloseWindow();
        return 1;
    }

    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    Vector3 camOffset = { 0.0f, 10.0f, 18.0f };

    Model computerModel = LoadModel("src/renderer/objects/computer.obj");
    Model computerScreenModel = LoadModel("src/renderer/objects/computer-screen.obj");
    float computerRot = 180.0f;

    Model bugModel = LoadModel("src/renderer/objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSizeY = bugBounds.max.y - bugBounds.min.y;
    PLAYER_RADIUS = bugSizeY / 2.0f;

    Vector3 bugPos = level.playerStart;
    float bugRot = 0.0f;

    float moveSpeed = 0.15f * (60.0f / (float)fps);
    float diagMoveSpeed = sqrt(0.5f) * moveSpeed;

    float verticalVelocity = 0.0f;
    const float gravity = 0.017f * (60.0f / (float)fps);
    const float jumpStrength = 0.76f * (60.0f / (float)fps);
    bool onGround = true;

    string collisionText = "Colliding with: none";
    string statusText = "";

    ComputerTerminal terminal;

    SetTargetFPS(fps);

    const char* title =
        "$$$$$$$\\\\ $$$$$$$$\\\\$$$$$$$\\\\  $$\\\\  $$\\\\  $$$$$$\\\\\n"
        "$$  __$$\\\\$$  _____|$$  __$$\\\\ $$ |  $$ |$$  __ $$\\\\\n"
        "$$ |  $$ |$$ |      $$ |  $$  |$$ |  $$ |$$ /  \\\\__|\n"
        "$$ |  $$ |$$$$$\\\\   $$$$$$$\\\\ |$$ |  $$ |$$ |  $$$$\\\\\n"
        "$$ |  $$ |$$  __|   $$  __$$\\\\ $$ |  $$ |$$ |  \\\\_$$ |\n"
        "$$ |  $$ |$$ |      $$ |  $$  |$$ |  $$ |$$ |     $$ |\n"
        "$$$$$$$  |$$$$$$$$\\\\$$$$$$$   |\\\\$$$$$$  |\\\\$$$$$$  |\n"
        "\\_______/ \\________|\\______/   \\______/  \\________/";

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ENTER)) {
            PlaySound(key);
            break;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            int lineHeight = 18;
            int titleFontSize = 16;
            int fixedCharWidth = MeasureText("/", titleFontSize);

            int maxCols = 0;
            int currentCols = 0;
            for (const char* p = title; *p != '\0'; p++) {
                if (*p == '\n') {
                    if (currentCols > maxCols) maxCols = currentCols;
                    currentCols = 0;
                } else {
                    currentCols++;
                }
            }
            if (currentCols > maxCols) maxCols = currentCols;

            int startX = GetScreenWidth() / 2 - (maxCols * fixedCharWidth) / 2;
            int y = GetScreenHeight() / 2 - 80;

            int row = 0;
            int col = 0;
            for (const char* p = title; *p != '\0'; p++) {
                if (*p == '\n') {
                    row++;
                    col = 0;
                } else {
                    if (*p != ' ') {
                        char cStr[2] = { *p, '\0' };
                        DrawText(cStr, startX + col * fixedCharWidth, y + row * lineHeight, titleFontSize, GREEN);
                    }
                    col++;
                }
            }

            if ((int)(GetTime() * 2) % 2 == 0) {
                DrawText("press enter to play",
                    GetScreenWidth() / 2 - MeasureText("press enter to play", 18) / 2,
                    GetScreenHeight() / 2 + 80,
                    18, DARKGREEN);
            }
        EndDrawing();
    }

    showLevelTitle(level.name);

    int currentLevel = 1;

    Color screenColor = RED;

    while (!WindowShouldClose()) {
        statusText.clear();

        vector<BoundingBox> platformBounds = buildPlatformBounds(level);
        vector<BoundingBox> solidBounds = buildSolidBounds(level);
        BoundingBox computerBox = getComputerBounds(level);
        BoundingBox doorBox = getDoorBounds(level);

        if (terminal.isOpen()) {
            terminal.update();
        } else {
            bool keyW = IsKeyDown(KEY_W);
            bool keyA = IsKeyDown(KEY_A);
            bool keyS = IsKeyDown(KEY_S);
            bool keyD = IsKeyDown(KEY_D);

            if ((keyW || keyA || keyS || keyD) && onGround) {
                if (!IsSoundPlaying(walkSound)) {
                    PlaySound(walkSound);
                }
            } else {
                StopSound(walkSound);
            }

            float currMoveSpeed = moveSpeed
                - (IsKeyDown(KEY_LEFT_CONTROL) * moveSpeed * 0.5f)
                + (IsKeyDown(KEY_LEFT_SHIFT) * moveSpeed);

            if ((keyW || keyS) && (keyA || keyD)) {
                currMoveSpeed = diagMoveSpeed
                    - (IsKeyDown(KEY_LEFT_CONTROL) * moveSpeed * 0.5f)
                    + (IsKeyDown(KEY_LEFT_SHIFT) * moveSpeed);
            }

            if (onGround && IsKeyPressed(KEY_SPACE)) {
                verticalVelocity = jumpStrength;
                PlaySound(jumpSound);
                onGround = false;
            }

            Vector3 zTestPos = bugPos;
            if (keyW) zTestPos.z -= currMoveSpeed;
            if (keyS) zTestPos.z += currMoveSpeed;

            if (!collidesWithSolidSideways(zTestPos, solidBounds)) {
                bugPos.z = zTestPos.z;
            }

            Vector3 xTestPos = bugPos;
            if (keyA) xTestPos.x -= currMoveSpeed;
            if (keyD) xTestPos.x += currMoveSpeed;

            if (!collidesWithSolidSideways(xTestPos, solidBounds)) {
                bugPos.x = xTestPos.x;
            }

            if (keyW && keyA) {
                bugRot = 45.0f;
            } else if (keyW && keyD) {
                bugRot = 315.0f;
            } else if (keyS && keyA) {
                bugRot = 135.0f;
            } else if (keyS && keyD) {
                bugRot = 225.0f;
            } else if (keyW) {
                bugRot = 0.0f;
            } else if (keyA) {
                bugRot = 90.0f;
            } else if (keyS) {
                bugRot = 180.0f;
            } else if (keyD) {
                bugRot = 270.0f;
            }

            verticalVelocity -= gravity;
            float proposedY = bugPos.y + verticalVelocity;

            bool landedOnSomething = false;
            BoundingBox proposedPlayerBox = makePlayerBox({ bugPos.x, proposedY, bugPos.z });

            for (const BoundingBox& box : solidBounds) {
                if (CheckCollisionBoxes(proposedPlayerBox, box)) {
                    float boxTop = box.max.y;
                    float boxBottom = box.min.y;

                    float oldBottom = bugPos.y - PLAYER_RADIUS;
                    float newBottom = proposedY - PLAYER_RADIUS;
                    float oldTop = bugPos.y + PLAYER_RADIUS;
                    float newTop = proposedY + PLAYER_RADIUS;

                    if (verticalVelocity <= 0.0f &&
                        oldBottom >= boxTop - 0.25f &&
                        newBottom <= boxTop + 0.25f) {
                        bugPos.y = boxTop + PLAYER_RADIUS;
                        verticalVelocity = 0.0f;
                        onGround = true;
                        landedOnSomething = true;
                        break;
                    }

                    if (verticalVelocity > 0.0f &&
                        oldTop <= boxBottom + 0.25f &&
                        newTop >= boxBottom - 0.25f) {
                        bugPos.y = boxBottom - PLAYER_RADIUS;
                        verticalVelocity = 0.0f;
                        landedOnSomething = true;
                        break;
                    }
                }
            }

            if (!landedOnSomething) {
                bugPos.y = proposedY;
                onGround = false;
            }

            float floorPlayerY = FLOOR_Y + PLAYER_RADIUS;
            if (bugPos.y <= floorPlayerY) {
                bugPos.y = floorPlayerY;
                verticalVelocity = 0.0f;
                onGround = true;
            }

            boundPos(bugPos, -48.0f, 48.0f, FLOOR_Y + PLAYER_RADIUS, 80.0f, -48.0f, 78.0f);

            BoundingBox playerBox = makePlayerBox(bugPos);
            bool hitComputer = isTouchingBox(playerBox, computerBox);
            bool hitDoor = isTouchingBox(playerBox, doorBox);
            bool hitPlatform = false;

            string roomId = "challenge" + to_string(currentLevel);
            Room* room = terminal.getGameState().getRoom(roomId);
            bool doorUnlocked = room && room->doorUnlocked;

            if (doorUnlocked) {
                screenColor = GREEN;
            } else {
                screenColor = RED;
            }

            if (hitDoor && doorUnlocked) {
                currentLevel++;
                PlaySound(doorSound);
                std::string nextFile = "src/assets/rooms/room" + std::to_string(currentLevel) + ".txt";
                if (!loadLevelFile(nextFile, level)) {
                    statusText = "You win!";
                    currentLevel--;
                } else {
                    showLevelTitle(level.name);
                    bugPos = level.playerStart;
                    verticalVelocity = 0.0f;
                    onGround = false;
                }
            } else if (hitDoor && !doorUnlocked) {
                statusText = "Door is locked.";
            }

            for (const BoundingBox& box : platformBounds) {
                if (isTouchingBox(playerBox, box)) {
                    hitPlatform = true;
                    break;
                }
            }

            if (hitComputer && hitDoor) {
                collisionText = "Colliding with: computer and door";
            } else if (hitComputer) {
                collisionText = "Colliding with: computer";
            } else if (hitDoor) {
                collisionText = "Colliding with: door";
            } else if (hitPlatform) {
                collisionText = "Colliding with: platform";
            } else {
                collisionText = "Colliding with: none";
            }

            if (hitComputer && IsKeyPressed(KEY_E)) {
                PlaySound(computerSound);
                terminal.open(roomId);
            }
        }

        Vector3 desiredCamPos = {
            bugPos.x + camOffset.x,
            bugPos.y + camOffset.y,
            bugPos.z + camOffset.z
        };

        boundPos(desiredCamPos, -45.0f, 45.0f, -40.0f, 45.0f, -30.0f, 78.0f);
        camera.position = desiredCamPos;
        camera.target = { bugPos.x, bugPos.y + 1.0f, bugPos.z };

        BeginDrawing();

        if (terminal.isOpen()) {
            ClearBackground(BLACK);
            terminal.draw();
        } else {
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

            for (const CloudObject& c : level.clouds) {
                drawCloud(c.position);
            }

            for (const PlaneObject& plane : level.planes) {
                DrawPlane(plane.position, plane.size, plane.color);
            }

            for (const ToriiGateObject& t : level.toriiGates) {
                drawToriiGate(t.basePos);
            }

            for (const BackgroundBlossomTreeObject& t : level.backgroundBlossomTrees) {
                drawBackgroundBlossomTree(t.trunkPos, t.trunkHeight);
            }

            for (const LanternObject& l : level.lanterns) {
                drawLantern(l.basePos);
            }

            for (const PetalPileObject& p : level.petalPiles) {
                drawPetalPile(p.position);
            }

            for (const BoxObject& box : level.boxes) {
                if (isRoomShellBox(box)) {
                    DrawCubeWiresV(box.position, box.size, box.color);
                } else {
                    DrawCubeV(box.position, box.size, box.color);
                    DrawCubeWiresV(box.position, box.size, BLACK);
                }
            }

            for (const CylinderExObject& cyl : level.cylinders) {
                DrawCylinderEx(
                    cyl.startPos,
                    cyl.endPos,
                    cyl.startRadius,
                    cyl.endRadius,
                    cyl.sides,
                    cyl.color
                );
            }

            for (const SphereObject& s : level.spheres) {
                DrawSphere(s.position, s.radius, s.color);
            }

            for (const SeaweedObject& s : level.seaweed) {
                drawSeaweed(s.basePos, s.color);
            }

            for (const BubbleColumnObject& b : level.bubbleColumns) {
                drawBubbleColumn(b.basePos, b.timeOffset);
            }

            for (const FishObject& f : level.fish) {
                drawFish(f.position, f.color, f.facing);
            }

            for (const JellyfishObject& j : level.jellyfish) {
                drawJellyfish(j.position, j.color);
            }

            for (const CatInBoxObject& c : level.catInBoxes) {
                drawCatInBox(c.position, c.catColor);
            }

            DrawModelEx(
                computerModel,
                level.computer.position,
                (Vector3){ 0.0f, 1.0f, 0.0f },
                computerRot,
                (Vector3){ 1.0f, 1.0f, 1.0f },
                WHITE
            );

            DrawModelEx(
                computerScreenModel,
                level.computer.position,
                (Vector3){ 0.0f, 1.0f, 0.0f },
                computerRot,
                (Vector3){ 1.0f, 1.0f, 1.0f },
                screenColor
            );

            DrawCubeV(level.door.position, level.door.size, BLUE);
            DrawCubeWiresV(level.door.position, level.door.size, BLACK);

            DrawModelEx(
                bugModel,
                bugPos,
                (Vector3){ 0.0f, 1.0f, 0.0f },
                bugRot,
                (Vector3){ 1.0f, 1.0f, 1.0f },
                BROWN
            );

            EndMode3D();

            DrawFPS(10, 10);
            DrawText(collisionText.c_str(), 10, 35, 20, BLACK);
            DrawText(onGround ? "Grounded" : "Airborne", 10, 60, 20, BLACK);
            DrawText("WASD move | SPACE jump | E use computer", 10, 85, 20, BLACK);

            if (!statusText.empty()) {
                DrawText(statusText.c_str(), 10, 110, 20, DARKGREEN);
            }

            BoundingBox playerBox = makePlayerBox(bugPos);
            if (isTouchingBox(playerBox, computerBox)) {
                DrawText("Press E to use computer", 10, 135, 20, DARKGREEN);
            }
        }

        EndDrawing();
    }

    StopSound(walkSound);

    UnloadSound(walkSound);
    UnloadSound(jumpSound);
    UnloadSound(computerSound);
    UnloadSound(doorSound);
    UnloadSound(key);

    UnloadModel(computerModel);
    UnloadModel(computerScreenModel);
    UnloadModel(bugModel);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

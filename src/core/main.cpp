#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "headers/LevelLoader.h"
#include "headers/ComputerTerminal.h"

using namespace std;

const float PLAYER_RADIUS = 2.0f;
const float FLOOR_Y = -50.0f;

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

vector<BoundingBox> buildPlatformBounds(const LevelData& level) {
    vector<BoundingBox> bounds;

    for (const BoxObject& box : level.boxes) {
        if (!isRoomShellBox(box)) {
            bounds.push_back(makeBoundingBox(box.position, box.size));
        }
    }

    return bounds;
}

vector<BoundingBox> buildSolidBounds(const LevelData& level) {
    vector<BoundingBox> solids = buildPlatformBounds(level);
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

bool collidesWithSolidSideways(Vector3 pos,
                               const vector<BoundingBox>& solidBounds) {
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

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 700;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");
    SetTargetFPS(fps);

    LevelData level;
    if (!loadLevelFile("src/assets/rooms/level1.txt", level)) {
        cout << "Failed to load level file.\n";
        CloseWindow();
        return 1;
    }

    Camera3D camera = { 0 };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 bugPos = {
        level.computer.position.x,
        FLOOR_Y + PLAYER_RADIUS,
        level.computer.position.z + 12.0f
    };

    Vector3 camOffset = { 0.0f, 10.0f, 18.0f };

    float moveSpeed = 0.15f * (60.0f / (float)fps);
    float diagMoveSpeed = sqrt(0.5f) * moveSpeed;

    float verticalVelocity = 0.0f;
    const float gravity = 0.015f * (60.0f / (float)fps);
    const float jumpStrength = 0.76f * (60.0f / (float)fps);
    bool onGround = true;

    string collisionText = "Colliding with: none";

    ComputerTerminal terminal;

    while (!WindowShouldClose()) {
        vector<BoundingBox> platformBounds = buildPlatformBounds(level);
        vector<BoundingBox> solidBounds = buildSolidBounds(level);
        BoundingBox computerBox = getComputerBounds(level);
        BoundingBox doorBox = getDoorBounds(level);

        BoundingBox playerBoxAtStart = makePlayerBox(bugPos);
        bool touchingComputer = isTouchingBox(playerBoxAtStart, computerBox);

        if (terminal.isOpen()) {
            terminal.update();
        } else {
            float currMoveSpeed = moveSpeed
                - (IsKeyDown(KEY_LEFT_SHIFT) * moveSpeed * 0.5f)
                + (IsKeyDown(KEY_LEFT_CONTROL) * moveSpeed);

            if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) &&
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) {
                currMoveSpeed = diagMoveSpeed
                    - (IsKeyDown(KEY_LEFT_SHIFT) * moveSpeed * 0.5f)
                    + (IsKeyDown(KEY_LEFT_CONTROL) * moveSpeed);
            }

            if (onGround && IsKeyPressed(KEY_SPACE)) {
                verticalVelocity = jumpStrength;
                onGround = false;
            }

            // Z movement
            Vector3 zTestPos = bugPos;
            if (IsKeyDown(KEY_W)) zTestPos.z -= currMoveSpeed;
            if (IsKeyDown(KEY_S)) zTestPos.z += currMoveSpeed;

            if (!collidesWithSolidSideways(zTestPos, solidBounds)) {
                bugPos.z = zTestPos.z;
            }

            // X movement
            Vector3 xTestPos = bugPos;
            if (IsKeyDown(KEY_A)) xTestPos.x -= currMoveSpeed;
            if (IsKeyDown(KEY_D)) xTestPos.x += currMoveSpeed;

            if (!collidesWithSolidSideways(xTestPos, solidBounds)) {
                bugPos.x = xTestPos.x;
            }

            // Vertical movement
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
                terminal.open("room_01");
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

            for (const BoxObject& box : level.boxes) {
                if (isRoomShellBox(box)) {
                    DrawCubeWiresV(box.position, box.size, box.color);
                } else {
                    DrawCubeV(box.position, box.size, box.color);
                    DrawCubeWiresV(box.position, box.size, BLACK);
                }
            }

            for (const PlaneObject& plane : level.planes) {
                DrawPlane(plane.position, plane.size, plane.color);
            }

            DrawCubeV(level.computer.position, COMPUTER_SIZE, BLACK);
            DrawCubeWiresV(level.computer.position, COMPUTER_SIZE, WHITE);

            DrawCubeV(level.door.position, DOOR_SIZE, BLUE);
            DrawCubeWiresV(level.door.position, DOOR_SIZE, BLACK);

            DrawSphere(bugPos, PLAYER_RADIUS, BROWN);

            EndMode3D();

            DrawFPS(10, 10);
            DrawText(collisionText.c_str(), 10, 35, 20, BLACK);
            DrawText(onGround ? "Grounded" : "Airborne", 10, 60, 20, BLACK);
            DrawText("WASD move | SPACE jump | E use computer", 10, 85, 20, BLACK);

            BoundingBox playerBox = makePlayerBox(bugPos);
            if (isTouchingBox(playerBox, computerBox)) {
                DrawText("Press E to use computer", 10, 110, 20, DARKGREEN);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
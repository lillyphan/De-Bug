#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "headers/LevelLoader.h"
#include "headers/ComputerTerminal.h"

using namespace std;

float PLAYER_RADIUS = 2.0f; // Made variable so it can adapt to the bug model's size
const float FLOOR_Y = -50.0f;

// Bound position within specific min/max coordinates
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

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1000;
    const int screenHeight = 700;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");

    // Load Level Data
    LevelData level;
    if (!loadLevelFile("src/assets/rooms/room1.txt", level)) {
        cout << "Failed to load level file.\n";
        CloseWindow();
        return 1;
    }

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
    camera.fovy = 60.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera mode type
    Vector3 camOffset = { 0.0f, 10.0f, 18.0f }; // Delta pos from bug

    // Load Models (Computer & Screen)
    Model computerModel = LoadModel("src/renderer/objects/computer.obj");
    Model computerScreenModel = LoadModel("src/renderer/objects/computer-screen.obj");
    float computerRot = 180;

    // Load Bug Model
    Model bugModel = LoadModel("src/renderer/objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSize[3] = {
        bugBounds.max.x - bugBounds.min.x,
        bugBounds.max.y - bugBounds.min.y,
        bugBounds.max.z - bugBounds.min.z
    }; // x, y, z
    PLAYER_RADIUS = bugSize[1] / 2.0f; // Update collision radius to match visual model height

    Vector3 bugPos = level.playerStart; // Spawn point
    float bugRot = 0;

    // Movement & Physics variables
    float moveSpeed = 0.15f * (60.0f / (float)fps); // Division is to have same speed at any fps
    float diagMoveSpeed = sqrt(0.5f) * moveSpeed; // Diagonal normalization

    float verticalVelocity = 0.0f;
    const float gravity = 0.015f * (60.0f / (float)fps);
    const float jumpStrength = 0.76f * (60.0f / (float)fps);
    bool onGround = true;

    string collisionText = "Colliding with: none";
    ComputerTerminal terminal;

    SetTargetFPS(fps); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // --- ASCII splash screen ---
    const char* title = 
        "  ____  U _____ u  ____     _   _    ____   \n"
        " |  _\"\\  \\| ___\"|/ |  _\"\\  | | | |  / __\"| \n"
        "/| | | |  |  _|\"  /| | | | | |_| | <\\___ \\ \n"
        "U| |_| |\\ | |___  U| |_| |\\|_   _|  u___) |\n"
        " |____/ u |_____|  |____/ u  |_|    |____/ u\n"
        "  |||_   <<   >>    |||_  .-,//|(_   )(  (( \n"
        " (__)_) (__) (__)  (__)_)  \\_)-'_/  (__)  ) \n";

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ENTER)) break;

        BeginDrawing();
            ClearBackground(BLACK);

            // Draw ASCII title
            int lineHeight = 18;
            int fontSize   = 16;
            int y          = GetScreenHeight()/2 - 80;

            // Split and draw line by line so we can center each one
            const char* ptr = title;
            char lineBuf[128];
            int lineIndex = 0;
            while (*ptr) {
                int len = 0;
                while (*ptr && *ptr != '\n') {
                    lineBuf[len++] = *ptr++;
                }
                lineBuf[len] = '\0';
                if (*ptr == '\n') ptr++;

                int x = GetScreenWidth()/2 - MeasureText(lineBuf, fontSize)/2;
                DrawText(lineBuf, x, y + lineIndex * lineHeight, fontSize, GREEN);
                lineIndex++;
            }

            // Blinking "press enter to play"
            if ((int)(GetTime() * 2) % 2 == 0) {
                DrawText("press enter to play",
                    GetScreenWidth()/2 - MeasureText("press enter to play", 18)/2,
                    GetScreenHeight()/2 + 60,
                    18, DARKGREEN);
            }

        EndDrawing();
    }

    showLevelTitle(level.name);

    int currentLevel = 1;

    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        // Update
        //----------------------------------------------------------------------------------
        vector<BoundingBox> platformBounds = buildPlatformBounds(level);
        vector<BoundingBox> solidBounds = buildSolidBounds(level);
        BoundingBox computerBox = getComputerBounds(level);
        BoundingBox doorBox = getDoorBounds(level);

        BoundingBox playerBoxAtStart = makePlayerBox(bugPos);
        bool touchingComputer = isTouchingBox(playerBoxAtStart, computerBox);

        if (terminal.isOpen()) {
            terminal.update();
        } else {
            bool keyW = IsKeyDown(KEY_W);
            bool keyA = IsKeyDown(KEY_A);
            bool keyS = IsKeyDown(KEY_S);
            bool keyD = IsKeyDown(KEY_D);

            float currMoveSpeed = moveSpeed
                - (IsKeyDown(KEY_LEFT_CONTROL) * moveSpeed * 0.5f)
                + (IsKeyDown(KEY_LEFT_SHIFT) * moveSpeed);

            // Check if moving diagonal
            if ((keyW || keyS) && (keyA || keyD)) {
                currMoveSpeed = diagMoveSpeed
                    - (IsKeyDown(KEY_LEFT_CONTROL) * moveSpeed * 0.5f)
                    + (IsKeyDown(KEY_LEFT_SHIFT) * moveSpeed);
            }

            // Jumping Logic
            if (onGround && IsKeyPressed(KEY_SPACE)) {
                verticalVelocity = jumpStrength;
                onGround = false;
            }

            // Z movement Logic
            Vector3 zTestPos = bugPos;
            if (keyW) zTestPos.z -= currMoveSpeed;
            if (keyS) zTestPos.z += currMoveSpeed;

            if (!collidesWithSolidSideways(zTestPos, solidBounds)) {
                bugPos.z = zTestPos.z;
            }

            // X movement Logic
            Vector3 xTestPos = bugPos;
            if (keyA) xTestPos.x -= currMoveSpeed;
            if (keyD) xTestPos.x += currMoveSpeed;

            if (!collidesWithSolidSideways(xTestPos, solidBounds)) {
                bugPos.x = xTestPos.x;
            }

            // Rotation Logic
            // We check two-key combinations first to ensure "ONLY" those are active
            if (keyW && keyA) {
                bugRot = 45;
            } else if (keyW && keyD) {
                bugRot = 315;
            } else if (keyS && keyA) {
                bugRot = 135;
            } else if (keyS && keyD) {
                bugRot = 225;
            }
            // If no two-key combo is happening, check for single keys
            else if (keyW) {
                bugRot = 0;
            } else if (keyA) {
                bugRot = 90;
            } else if (keyS) {
                bugRot = 180;
            } else if (keyD) {
                bugRot = 270;
            }

            // Vertical movement Logic
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

            // Bound the bug
            boundPos(bugPos, -48.0f, 48.0f, FLOOR_Y + PLAYER_RADIUS, 80.0f, -48.0f, 78.0f);

            BoundingBox playerBox = makePlayerBox(bugPos);
            bool hitComputer = isTouchingBox(playerBox, computerBox);
            bool hitDoor = isTouchingBox(playerBox, doorBox);
            bool hitPlatform = false;

            // Check if puzzle was just solved — unlock the door
            std::string roomId = "challenge" + std::to_string(currentLevel);
            Room* room = terminal.getGameState().getRoom(roomId);
            bool doorUnlocked = room && room->doorUnlocked;

            if (hitDoor && doorUnlocked) {
                currentLevel++;
                std::string nextFile = "src/assets/rooms/room" + std::to_string(currentLevel) + ".txt";
                if (!loadLevelFile(nextFile, level)) {
                    // no more levels, win screen!!
                } else {
                    showLevelTitle(level.name);
                    // reset player position to near the computer
                    // bugPos = {
                    //     level.computer.position.x,
                    //     FLOOR_Y + PLAYER_RADIUS,
                    //     level.computer.position.z + 12.0f
                    // };
                    // terminal.open("challenge" + std::to_string(currentLevel));
                }
            } else if (hitDoor && !doorUnlocked) {
                DrawText("Door is locked.", 10, 110, 20, DARKGREEN);
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
                terminal.open(roomId);
            }
        }

        // Camera to follow bug
        Vector3 desiredCamPos = {
            bugPos.x + camOffset.x,
            bugPos.y + camOffset.y,
            bugPos.z + camOffset.z
        };

        // Bound the cam to box limits
        boundPos(desiredCamPos, -45.0f, 45.0f, -40.0f, 45.0f, -30.0f, 78.0f);
        camera.position = desiredCamPos;
        camera.target = { bugPos.x, bugPos.y + 1.0f, bugPos.z };

        // --------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        if (terminal.isOpen()) {
            ClearBackground(BLACK);
            terminal.draw();
        } else {
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

            // Draw level architecture
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

            // Draw objects with Default Raylib Shader
            DrawModelEx(computerModel, level.computer.position, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
            DrawModelEx(computerScreenModel, level.computer.position, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, GREEN);

            DrawCubeV(level.door.position, DOOR_SIZE, BLUE);
            DrawCubeWiresV(level.door.position, DOOR_SIZE, BLACK);

            // Draw bug model replacing the physics sphere
            DrawModelEx(bugModel, bugPos, (Vector3){ 0.0f, 1.0f, 0.0f }, bugRot, (Vector3){ 1.0f, 1.0f, 1.0f }, BROWN);

            EndMode3D();

            std::string roomId = "challenge" + std::to_string(currentLevel);
            Room* room = terminal.getGameState().getRoom(roomId);

            // Draw UI HUD
            DrawFPS(10, 10);
            DrawText(collisionText.c_str(), 10, 35, 20, BLACK);
            if (roomId == "challenge1")
            DrawText("WASD move | SPACE jump | SHIFT sprint | CTRL sneak", 20, 655, 20, BLACK);
            DrawText(onGround ? "Grounded" : "Airborne", 10, 60, 20, BLACK);

            BoundingBox playerBox = makePlayerBox(bugPos);
            if (isTouchingBox(playerBox, computerBox)) {
                DrawText("Press E to use computer", 10, 110, 20, DARKGREEN);
            }
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(computerModel);
    UnloadModel(computerScreenModel);
    UnloadModel(bugModel);

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

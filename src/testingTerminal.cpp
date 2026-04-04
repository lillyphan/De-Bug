#include <cmath>
#include <iostream>
#include <string>
#include "raylib.h"
#include "raymath.h"
#include "core/headers/LevelLoader.h"

using namespace std;

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

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug Level Test");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 10.0f };
    camera.target   = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy     = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Fixed player/camera start
    Vector3 bugPos = { 0.0f, -48.0f, 45.0f };
    Vector3 camOffset = { 0.0f, 10.0f, 20.0f };

    float bugSpeed = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrt(0.5f * bugSpeed * bugSpeed);

    LevelData level;
    if (!loadLevelFile("src/assets/level1.txt", level)) {
        cout << "Failed to load level file.\n";
        CloseWindow();
        return 1;
    }

    string collisionText = "Colliding with: none";

    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        float currBugSpeed = bugSpeed
            - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 0.5f)
            + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);

        if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) &&
            (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) {
            currBugSpeed = diagBugSpeed
                - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 0.5f)
                + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);
        }

        if (IsKeyDown(KEY_W)) bugPos.z -= currBugSpeed;
        if (IsKeyDown(KEY_A)) bugPos.x -= currBugSpeed;
        if (IsKeyDown(KEY_S)) bugPos.z += currBugSpeed;
        if (IsKeyDown(KEY_D)) bugPos.x += currBugSpeed;

        boundPos(bugPos, -50.0f, 50.0f, -100.0f, 50.0f, -50.0f, 80.0f);

        Vector3 currCamPos = {
            camOffset.x + bugPos.x,
            camOffset.y + bugPos.y,
            camOffset.z + bugPos.z
        };

        boundPos(currCamPos, -45.0f, 45.0f, -45.0f, 45.0f, -45.0f, 80.0f);

        camera.position = currCamPos;
        camera.target = bugPos;

        BoundingBox playerBox = makeBoundingBox(bugPos, { 4.0f, 4.0f, 4.0f });

        bool hitComputer = CheckCollisionBoxes(playerBox, getComputerBounds(level));
        bool hitDoor = CheckCollisionBoxes(playerBox, getDoorBounds(level));

        if (hitComputer) {
            collisionText = "Colliding with: " + level.computer.id;
        } else if (hitDoor) {
            collisionText = "Colliding with: " + level.door.id;
        } else {
            collisionText = "Colliding with: none";
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        for (const BoxObject& box : level.boxes) {
            DrawCubeV(box.position, box.size, box.color);
            DrawCubeWiresV(box.position, box.size, BLACK);
        }

        for (const PlaneObject& plane : level.planes) {
            DrawPlane(plane.position, plane.size, plane.color);
        }

        DrawCubeV(level.computer.position, COMPUTER_SIZE, BLACK);
        DrawCubeWiresV(level.computer.position, COMPUTER_SIZE, WHITE);

        DrawCubeV(level.door.position, DOOR_SIZE, BLUE);
        DrawCubeWiresV(level.door.position, DOOR_SIZE, BLACK);

        DrawSphere(bugPos, 2.0f, BROWN);

        EndMode3D();

        DrawFPS(10, 10);
        DrawText(collisionText.c_str(), 10, 35, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
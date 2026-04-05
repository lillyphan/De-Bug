#include <cmath>
#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"

using namespace std;

// var ptr, {x lower, x upper}, {y lower, y upper}, {z lower, z upper}
void boundPos(Vector3 &var, float x[2], float y[2], float z[2]) {
    if (var.x > x[1]) {
        var.x = x[1];
    }
    if (var.x < x[0]) {
        var.x = x[0];
    }
    if (var.y > y[1]) {
        var.y = y[1];
    }
    if (var.y < y[0]) {
        var.y = y[0];
    }
    if (var.z > z[1]) {
        var.z = z[1];
    }
    if (var.z < z[0]) {
        var.z = z[0];
    }
}

struct Platform {
    Vector3 position;
    Vector3 size;
    Color color;
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");

    // Camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 75.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Player and camera offset
    Vector3 bugPos = { -38.0f, -48.0f, -18.0f };
    Vector3 camPos = { 0.0f, 30.0f, 55.0f };

    float bugSpeed = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrt(0.5f * bugSpeed * bugSpeed);

    // Level 1 platforms
    // bottom-left to top-right staircase path
    vector<Platform> platforms = {
        {{ -32.0f, -42.0f, -24.0f }, { 14.0f, 2.0f, 14.0f }, DARKGRAY},
        {{ -10.0f, -30.0f, -30.0f }, { 14.0f, 2.0f, 14.0f }, DARKGRAY},
        {{  12.0f, -18.0f, -36.0f }, { 14.0f, 2.0f, 14.0f }, DARKGRAY},
        {{  30.0f,  -6.0f, -42.0f }, { 14.0f, 2.0f, 14.0f }, DARKGRAY}
    };

    SetTargetFPS(fps);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        float currBugSpeed = bugSpeed
            - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1 / 2)
            + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);

        if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) &&
            (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) {
            currBugSpeed = diagBugSpeed
                - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1 / 2)
                + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);
        }

        if (IsKeyDown(KEY_W)) {
            bugPos.z -= currBugSpeed;
        }
        if (IsKeyDown(KEY_A)) {
            bugPos.x -= currBugSpeed;
        }
        if (IsKeyDown(KEY_S)) {
            bugPos.z += currBugSpeed;
        }
        if (IsKeyDown(KEY_D)) {
            bugPos.x += currBugSpeed;
        }

        boundPos(bugPos, (float[2]){-50, 50}, (float[2]){-100, 50}, (float[2]){-50, 50});

        Vector3 currCamPos = {
            camPos.x + bugPos.x,
            camPos.y + bugPos.y,
            camPos.z + bugPos.z
        };

        boundPos(currCamPos, (float[2]){-80, 80}, (float[2]){-80, 80}, (float[2]){-80, 120});

        camera.position = currCamPos;
        camera.target = (Vector3){ bugPos.x, bugPos.y + 8.0f, bugPos.z - 10.0f };

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Room bounds
        DrawCubeWiresV({ 0, 0, 0 }, { 100, 100, 100 }, BLACK);
        DrawCubeWiresV({ 0, 0, 0 }, { 200, 200, 200 }, RED);

        // Floor
        DrawPlane({ 0, -50, 0 }, { 100, 100 }, GRAY);
        DrawPlane({ 0, -50, 75 }, { 100, 50 }, GRAY);

        // Platforms
        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);
        }

        // Door on the top-right platform
        DrawCubeV({ 36.0f, -1.0f, -42.0f }, { 4.0f, 8.0f, 1.5f }, BLUE);
        DrawCubeWiresV({ 36.0f, -1.0f, -42.0f }, { 4.0f, 8.0f, 1.5f }, BLACK);

        // Player
        DrawSphere(bugPos, 2, BROWN);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
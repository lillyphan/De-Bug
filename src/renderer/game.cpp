#include <cmath>
#include <iostream>
#include "raylib.h"
#include "raymath.h"
using namespace std;

// var ptr, {x lower, x upper}, {y lower, y upper}, {z lower, z upper}
void boundPos(Vector3 &var, float x[2], float y[2], float z[2]) {
    if (var.x > x[1]) {
        var.x = x[1];
    }
    if (var.x < x[0]) {
        var.x =x[0];
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

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f }; // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
    camera.fovy = 60.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera mode type
    float camSize[2] = {75, (9/16*75)};
    Vector3 camPos = { 0.0f, 10.0f, 20.0f }; // Delta pos from bug

    Model computerModel = LoadModel("objects/computer.obj");
    Model computerScreenModel = LoadModel("objects/computer-screen.obj");
    BoundingBox computerBounds = GetMeshBoundingBox(computerModel.meshes[0]);
    float computerSize[3] = {
        computerBounds.max.x - computerBounds.min.x,
        computerBounds.max.y - computerBounds.min.y,
        computerBounds.max.z - computerBounds.min.z
    }; // x, y, z
    Vector3 computerPos = { 0.0f, (-50.0f+computerSize[1]/2), 0.0f };
    float computerRot = 180;

    Model bugModel = LoadModel("objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSize[3] = {
        bugBounds.max.x - bugBounds.min.x,
        bugBounds.max.y - bugBounds.min.y,
        bugBounds.max.z - bugBounds.min.z
    }; // x, y, z
    Vector3 bugPos = { 0.0f, (-50.0f+bugSize[1]/2), 45.0f }; // Spawn point
    float bugRot = 0;
    float bugSpeed = 0.15*(60/(float)fps); // Division is to have same speed at any fps
    // For when two directions are being held at the same time for it to move the same speed
    float diagBugSpeed = sqrt(0.5*bugSpeed*bugSpeed);

    SetTargetFPS(fps); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        // Update
        //----------------------------------------------------------------------------------
        bool keyW = IsKeyDown(KEY_W);
        bool keyA = IsKeyDown(KEY_A);
        bool keyS = IsKeyDown(KEY_S);
        bool keyD = IsKeyDown(KEY_D);

        float currBugSpeed = bugSpeed - (
            IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2
            ) + (
            IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed
        );

        // Check if moving diagonal
        if ((keyW || keyS) && (keyA || keyD)) {
            currBugSpeed = diagBugSpeed - (
                IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2
            ) + (
                IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed
            );
        }

        // Movement Logic
        if (keyW) {
            bugPos.z -= currBugSpeed;
        }
        if (keyA) {
            bugPos.x -= currBugSpeed;
        }
        if (keyS) {
            bugPos.z += currBugSpeed;
        }
        if (keyD) {
            bugPos.x += currBugSpeed;
        }

        // Rotation Logic (New code based on your whiteboard)
        // We check two-key combinations first to ensure "ONLY" those are active
        if (keyW && keyA) {
            bugRot = 45;
        }
        else if (keyW && keyD) {
            bugRot = 315;
        }
        else if (keyS && keyA) {
            bugRot = 135;
        }
        else if (keyS && keyD) {
            bugRot = 225;
        }
        // If no two-key combo is happening, check for single keys
        else if (keyW) {
            bugRot = 0;
        }
        else if (keyA) {
            bugRot = 90;
        }
        else if (keyS) {
            bugRot = 180;
        }
        else if (keyD) {
            bugRot = 270;
        }

        // Bound the bug
        boundPos(
            bugPos,
            (float[2]){-50+bugSize[0]/2, 50-bugSize[0]/2},
            (float[2]){-100+bugSize[1]/2, 50-bugSize[1]/2},
            (float[2]){-50+bugSize[2]/2, 50-bugSize[2]/2}
        );

        // Camera to follow bug
        Vector3 currCamPos = {camPos.x+bugPos.x, camPos.y+bugPos.y, camPos.z+bugPos.z};
        // Bound the cam to 90^2 box
        boundPos(currCamPos, (float[2]){-45,45}, (float[2]){-45,45}, (float[2]){-45,80});

        camera.position = currCamPos;
        camera.target = bugPos;

        // --------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, BLACK); // position, size, color
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, RED);

        // Draw objects with Default Raylib Shader
        DrawPlane({0,-50,0}, {100, 100}, GRAY); // centerPos, size, color
        DrawPlane({0,-50,75}, {100, 50}, GRAY); // OOB Floor
        //DrawSphere(bugPos, 2, BROWN); //Vector3 centerPos, float radius, Color color
        DrawModelEx(bugModel, bugPos, (Vector3){ 0.0f, 1.0f, 0.0f }, bugRot, (Vector3){ 1.0f, 1.0f, 1.0f }, BROWN);
        DrawModelEx(computerModel, computerPos, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
        DrawModelEx(computerScreenModel, computerPos, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, GREEN);

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

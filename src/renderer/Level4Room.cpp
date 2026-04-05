#include <cmath>
#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
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

struct Platform {
    Vector3 position;
    Vector3 size;
    Color color;
};

void drawRotatedPaper(Vector3 position, Vector3 size, float rotationDegrees,
                      Color fillColor, Color wireColor) {
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotationDegrees, 0.0f, 1.0f, 0.0f);

    DrawCubeV({ 0.0f, 0.0f, 0.0f }, size, fillColor);
    DrawCubeWiresV({ 0.0f, 0.0f, 0.0f }, size, wireColor);

    rlPopMatrix();
}


// Program main entry point

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    float camSize[2] = {75, (9/16*75)};
    Vector3 camPos = { 0.0f, 10.0f, 20.0f };

    Model computerModel = LoadModel("objects/computer.obj");
    Model computerScreenModel = LoadModel("objects/computer-screen.obj");
    BoundingBox computerBounds = GetMeshBoundingBox(computerModel.meshes[0]);
    float computerSize[3] = {
        computerBounds.max.x - computerBounds.min.x,
        computerBounds.max.y - computerBounds.min.y,
        computerBounds.max.z - computerBounds.min.z
    };
    Vector3 computerPos = { 18.0f, (-48.0f + 1.0f + computerSize[1]/2), 6.0f };
    float computerRot = 180;

    Model bugModel = LoadModel("objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSize[3] = {
        bugBounds.max.x - bugBounds.min.x,
        bugBounds.max.y - bugBounds.min.y,
        bugBounds.max.z - bugBounds.min.z
    };
    Vector3 bugPos = { -34.0f, (-24.0f + 1.0f + bugSize[1]/2), -18.0f };
    float bugRot = 0;
    float bugSpeed = 0.15*(60/(float)fps);
    float diagBugSpeed = sqrt(0.5*bugSpeed*bugSpeed);

    vector<Platform> platforms = {
        {{ -30.0f, -24.0f, -16.0f }, { 20.0f, 2.0f, 12.0f }, DARKGRAY},
        {{ -12.0f, -32.0f, -10.0f }, { 12.0f, 2.0f, 12.0f }, DARKGRAY},
        {{   4.0f, -40.0f,  -2.0f }, { 10.0f, 2.0f, 10.0f }, DARKGRAY},
        {{  22.0f, -48.0f,   8.0f }, { 18.0f, 2.0f, 14.0f }, DARKGRAY},
        {{  24.0f, -34.0f,  -6.0f }, { 16.0f, 2.0f, 10.0f }, DARKGRAY}
    };

    Vector3 doorPos = { 29.0f, -44.0f, 2.0f };
    Vector3 filePos = { 24.0f, -31.6f, -6.0f };

    SetTargetFPS(fps);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        bool keyW = IsKeyDown(KEY_W);
        bool keyA = IsKeyDown(KEY_A);
        bool keyS = IsKeyDown(KEY_S);
        bool keyD = IsKeyDown(KEY_D);

        float currBugSpeed = bugSpeed - (
            IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2
            ) + (
            IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed
        );

        if ((keyW || keyS) && (keyA || keyD)) {
            currBugSpeed = diagBugSpeed - (
                IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2
            ) + (
                IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed
            );
        }

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

        if (IsKeyDown(KEY_SPACE)) {
            bugPos.y += currBugSpeed;
        }

        if (IsKeyDown(KEY_C)) {
            bugPos.y -= currBugSpeed;
        }

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

        boundPos(
            bugPos,
            (float[2]){-50+bugSize[0]/2, 50-bugSize[0]/2},
            (float[2]){-100+bugSize[1]/2, 50-bugSize[1]/2},
            (float[2]){-50+bugSize[2]/2, 50-bugSize[2]/2}
        );

        Vector3 currCamPos = {camPos.x+bugPos.x, camPos.y+bugPos.y, camPos.z+bugPos.z};
        boundPos(currCamPos, (float[2]){-45,45}, (float[2]){-45,45}, (float[2]){-45,80});

        camera.position = currCamPos;
        camera.target = bugPos;

        float time = (float)GetTime();
        float fileSpin = time * 120.0f;
        float fileBob = sin(time * 2.5f) * 0.35f;

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, BLACK);
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, RED);

        DrawPlane({0,-50,0}, {100, 100}, GRAY);
        DrawPlane({0,-50,75}, {100, 50}, GRAY);

        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);
        }

        DrawModelEx(bugModel, bugPos, (Vector3){ 0.0f, 1.0f, 0.0f }, bugRot, (Vector3){ 1.0f, 1.0f, 1.0f }, BROWN);
        DrawModelEx(computerModel, computerPos, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
        DrawModelEx(computerScreenModel, computerPos, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, GREEN);

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, BLUE);
        DrawCubeWiresV(doorPos, { 4.0f, 8.0f, 1.5f }, BLACK);

        drawRotatedPaper(
            { filePos.x, filePos.y + fileBob, filePos.z },
            { 1.2f, 1.8f, 0.15f },
            fileSpin,
            WHITE,
            LIGHTGRAY
        );

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    // De-Initialization
    UnloadModel(computerModel);
    UnloadModel(computerScreenModel);
    UnloadModel(bugModel);
    CloseWindow();

    return 0;
}
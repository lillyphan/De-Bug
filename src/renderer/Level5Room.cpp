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

void drawCatInBox(Vector3 boxPos, Color catColor) {
    Color boxColor = { 181, 140, 99, 255 };

    // box
    DrawCubeV(boxPos, { 4.0f, 2.0f, 4.0f }, boxColor);
    DrawCubeWiresV(boxPos, { 4.0f, 2.0f, 4.0f }, WHITE);

    // head
    Vector3 headPos = { boxPos.x, boxPos.y + 1.8f, boxPos.z };
    DrawSphere(headPos, 1.2f, catColor);

    // left ear outer
    DrawCubeV({ headPos.x - 0.78f, headPos.y + 0.82f, headPos.z - 0.10f }, { 0.72f, 0.34f, 0.72f }, catColor);
    DrawCubeV({ headPos.x - 0.78f, headPos.y + 1.12f, headPos.z - 0.10f }, { 0.50f, 0.28f, 0.50f }, catColor);
    DrawCubeV({ headPos.x - 0.78f, headPos.y + 1.38f, headPos.z - 0.10f }, { 0.26f, 0.22f, 0.26f }, catColor);

    // right ear outer
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 0.82f, headPos.z - 0.10f }, { 0.72f, 0.34f, 0.72f }, catColor);
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 1.12f, headPos.z - 0.10f }, { 0.50f, 0.28f, 0.50f }, catColor);
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 1.38f, headPos.z - 0.10f }, { 0.26f, 0.22f, 0.26f }, catColor);

    // inner ears - on visible/front side
    DrawCubeV({ headPos.x - 0.78f, headPos.y + 1.03f, headPos.z + 0.28f }, { 0.26f, 0.26f, 0.26f }, PINK);
    DrawCubeV({ headPos.x + 0.78f, headPos.y + 1.03f, headPos.z + 0.28f }, { 0.26f, 0.26f, 0.26f }, PINK);

    // eyes - on visible/front side
    DrawSphere({ headPos.x - 0.38f, headPos.y + 0.20f, headPos.z + 1.22f }, 0.18f, BLACK);
    DrawSphere({ headPos.x + 0.38f, headPos.y + 0.20f, headPos.z + 1.22f }, 0.18f, BLACK);

    // nose - on visible/front side
    DrawSphere({ headPos.x, headPos.y - 0.08f, headPos.z + 1.26f }, 0.15f, BLACK);
}

void drawToriiGate(Vector3 basePos) {
    Color gateRed = { 170, 35, 35, 255 };
    Color darkWood = { 70, 45, 30, 255 };
    Color accentBlack = BLACK;

    // main pillars
    DrawCubeV({ basePos.x - 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, gateRed);
    DrawCubeWiresV({ basePos.x - 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, accentBlack);

    DrawCubeV({ basePos.x + 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, gateRed);
    DrawCubeWiresV({ basePos.x + 13.0f, basePos.y + 19.0f, basePos.z }, { 3.5f, 38.0f, 3.5f }, accentBlack);

    // lower crossbeam
    DrawCubeV({ basePos.x, basePos.y + 28.0f, basePos.z }, { 34.0f, 3.0f, 3.5f }, gateRed);
    DrawCubeWiresV({ basePos.x, basePos.y + 28.0f, basePos.z }, { 34.0f, 3.0f, 3.5f }, accentBlack);

    // top beam
    DrawCubeV({ basePos.x, basePos.y + 34.0f, basePos.z }, { 46.0f, 3.0f, 4.5f }, gateRed);
    DrawCubeWiresV({ basePos.x, basePos.y + 34.0f, basePos.z }, { 46.0f, 3.0f, 4.5f }, accentBlack);

    // top cap
    DrawCubeV({ basePos.x, basePos.y + 37.0f, basePos.z }, { 38.0f, 1.4f, 2.8f }, darkWood);
    DrawCubeWiresV({ basePos.x, basePos.y + 37.0f, basePos.z }, { 38.0f, 1.4f, 2.8f }, accentBlack);

    // center support
    DrawCubeV({ basePos.x, basePos.y + 22.0f, basePos.z }, { 2.3f, 12.0f, 2.3f }, darkWood);
    DrawCubeWiresV({ basePos.x, basePos.y + 22.0f, basePos.z }, { 2.3f, 12.0f, 2.3f }, accentBlack);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug Level 5");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    Vector3 camPos = { 0.0f, 10.0f, 20.0f };

    Model bugModel = LoadModel("objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSize[3] = {
        bugBounds.max.x - bugBounds.min.x,
        bugBounds.max.y - bugBounds.min.y,
        bugBounds.max.z - bugBounds.min.z
    };

    Model computerModel = LoadModel("objects/computer.obj");
    Model computerScreenModel = LoadModel("objects/computer-screen.obj");
    BoundingBox computerBounds = GetMeshBoundingBox(computerModel.meshes[0]);
    float computerSize[3] = {
        computerBounds.max.x - computerBounds.min.x,
        computerBounds.max.y - computerBounds.min.y,
        computerBounds.max.z - computerBounds.min.z
    };
    float computerRot = 180;

    Vector3 bugPos = { 0.0f, (-40.0f + 1.0f + bugSize[1] / 2.0f), 6.0f };
    float bugRot = 0;
    float bugSpeed = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrt(0.5f * bugSpeed * bugSpeed);

    vector<Platform> platforms = {
        // center spawn square
        {{   0.0f, -40.0f,   6.0f }, {  8.0f, 2.0f,  8.0f }, DARKGRAY},

        // right middle platform
        {{  18.0f, -34.0f,   2.0f }, { 12.0f, 2.0f,  8.0f }, DARKGRAY},

        // left platform below first blossom
        {{ -18.0f, -28.0f,   0.0f }, { 12.0f, 2.0f,  8.0f }, DARKGRAY},

        // left blossom top (+ shape) - pink, moved much higher
        {{ -26.0f, -14.0f, -10.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -32.0f, -14.0f, -10.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -20.0f, -14.0f, -10.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -26.0f, -14.0f,  -4.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -26.0f, -14.0f, -16.0f }, {  6.0f, 2.0f,  6.0f }, PINK},

        // left blossom trunk - much taller
        {{ -26.0f, -21.0f, -10.0f }, {  4.0f, 14.0f, 4.0f }, DARKBROWN},

        // left blossom trunk - dark brown, taller
        {{ -26.0f, -23.0f, -10.0f }, {  4.0f, 10.0f, 4.0f }, DARKBROWN},


        // right blossom trunk - dark brown
        {{  34.0f, -32.0f,  -2.0f }, {  4.0f, 14.0f,  4.0f }, DARKBROWN},

        // right blossom top (+ shape) - pink
        {{  34.0f, -23.0f,  -2.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{  28.0f, -23.0f,  -2.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{  40.0f, -23.0f,  -2.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{  34.0f, -23.0f,   4.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{  34.0f, -23.0f,  -8.0f }, {  6.0f, 2.0f,  6.0f }, PINK}
    };

        Vector3 computerPos = {
    18.0f,
    -34.0f + 1.0f + computerSize[1] / 2.0f,
    -2.0f
};

Vector3 doorPos = { -22.0f, -23.0f, -4.0f };

    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        bool keyW = IsKeyDown(KEY_W);
        bool keyA = IsKeyDown(KEY_A);
        bool keyS = IsKeyDown(KEY_S);
        bool keyD = IsKeyDown(KEY_D);

        float currBugSpeed = bugSpeed
            - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1 / 2)
            + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);

        if ((keyW || keyS) && (keyA || keyD)) {
            currBugSpeed = diagBugSpeed
                - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1 / 2)
                + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);
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
            (float[2]){-50 + bugSize[0] / 2, 50 - bugSize[0] / 2},
            (float[2]){-100 + bugSize[1] / 2, 50 - bugSize[1] / 2},
            (float[2]){-50 + bugSize[2] / 2, 50 - bugSize[2] / 2}
        );

        Vector3 currCamPos = {
            camPos.x + bugPos.x,
            camPos.y + bugPos.y,
            camPos.z + bugPos.z
        };

        boundPos(currCamPos, (float[2]){-45, 45}, (float[2]){-45, 45}, (float[2]){-45, 80});

        camera.position = currCamPos;
        camera.target = bugPos;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, BLACK);
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, RED);

        DrawPlane({0,-50,0}, {100, 100}, GRAY);
        DrawPlane({0,-50,75}, {100, 50}, GRAY);

        drawToriiGate({ 4.0f, -50.0f, -48.0f });
    

        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);
        }

        // white cat in box on left blossom
        drawCatInBox({ -26.0f, -10.5f, -10.0f }, WHITE);

        // orange cat in box on right blossom
        drawCatInBox({ 34.0f, -19.5f, -2.0f }, ORANGE);

        DrawModelEx(
            computerModel,
            computerPos,
            (Vector3){ 0.0f, 1.0f, 0.0f },
            computerRot,
            (Vector3){ 1.0f, 1.0f, 1.0f },
            WHITE
        );

        DrawModelEx(
            computerScreenModel,
            computerPos,
            (Vector3){ 0.0f, 1.0f, 0.0f },
            computerRot,
            (Vector3){ 1.0f, 1.0f, 1.0f },
            GREEN
        );

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, BLUE);
        DrawCubeWiresV(doorPos, { 4.0f, 8.0f, 1.5f }, BLACK);

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
        EndDrawing();
    }

    UnloadModel(computerModel);
    UnloadModel(computerScreenModel);
    UnloadModel(bugModel);
    CloseWindow();
    return 0;
}
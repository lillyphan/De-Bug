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
        {{   0.0f, -40.0f,   6.0f }, {  8.0f, 2.0f,  8.0f }, { 115, 108, 112, 255 }},
        {{  18.0f, -34.0f,   2.0f }, { 12.0f, 2.0f,  8.0f }, { 115, 108, 112, 255 }},
        {{ -18.0f, -28.0f,   0.0f }, { 12.0f, 2.0f,  8.0f }, { 115, 108, 112, 255 }},

        {{ -26.0f, -14.0f, -10.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -32.0f, -14.0f, -10.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -20.0f, -14.0f, -10.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -26.0f, -14.0f,  -4.0f }, {  6.0f, 2.0f,  6.0f }, PINK},
        {{ -26.0f, -14.0f, -16.0f }, {  6.0f, 2.0f,  6.0f }, PINK},

        {{ -26.0f, -21.0f, -10.0f }, {  4.0f, 14.0f, 4.0f }, DARKBROWN},

        {{  34.0f, -32.0f,  -2.0f }, {  4.0f, 14.0f,  4.0f }, DARKBROWN},

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
        ClearBackground({ 255, 236, 245, 255 });

        BeginMode3D(camera);

        drawCloud({ -30.0f, 0.0f, -30.0f });
        drawCloud({ 0.0f, 6.0f, -20.0f });
        drawCloud({ 26.0f, 1.0f, -34.0f });

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, Fade(BLACK, 0.35f));
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, Fade(RED, 0.18f));

        DrawPlane({0,-50,0}, {100, 100}, { 196, 224, 186, 255 });
        DrawPlane({0,-49.9f,75}, {100, 50}, { 186, 214, 176, 255 });

        drawToriiGate({ 4.0f, -50.0f, -48.0f });

        drawBackgroundBlossomTree({ -42.0f, -50.0f, -34.0f }, 16.0f);
        drawBackgroundBlossomTree({ -8.0f, -50.0f, -36.0f }, 18.0f);
        drawBackgroundBlossomTree({ 24.0f, -50.0f, -34.0f }, 17.0f);
        drawBackgroundBlossomTree({ 44.0f, -50.0f, -22.0f }, 14.0f);

        drawLantern({ -6.0f, -50.0f, -8.0f });
        drawLantern({ 8.0f, -50.0f, -18.0f });
        drawLantern({ 24.0f, -50.0f, 10.0f });
        drawLantern({ -30.0f, -50.0f, 10.0f });

        drawPetalPile({ -20.0f, -49.2f, -8.0f });
        drawPetalPile({ -4.0f, -49.2f, 4.0f });
        drawPetalPile({ 14.0f, -49.2f, 2.0f });
        drawPetalPile({ 28.0f, -49.2f, -4.0f });
        drawPetalPile({ 34.0f, -49.2f, 12.0f });
        drawPetalPile({ -36.0f, -49.2f, 0.0f });

        DrawSphere({ -34.0f, -48.5f, 18.0f }, 2.4f, { 120, 110, 118, 255 });
        DrawSphere({ -30.0f, -48.8f, 21.0f }, 1.6f, { 120, 110, 118, 255 });
        DrawSphere({ 40.0f, -48.6f, 16.0f }, 2.0f, { 120, 110, 118, 255 });
        DrawSphere({ 43.0f, -48.8f, 13.0f }, 1.4f, { 120, 110, 118, 255 });

        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);
        }

        DrawCubeV({ -18.0f, -27.6f, 0.0f }, { 13.5f, 0.3f, 9.5f }, Fade(PINK, 0.15f));
        DrawCubeV({ 18.0f, -33.6f, 2.0f }, { 13.5f, 0.3f, 9.5f }, Fade(PINK, 0.15f));

        drawCatInBox({ -26.0f, -10.5f, -10.0f }, WHITE);
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

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, { 80, 120, 180, 255 });
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
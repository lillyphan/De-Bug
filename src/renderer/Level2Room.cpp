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

void drawVine(Vector3 startPos, float length, float sway) {
    for (int i = 0; i < 8; i++) {
        float t = (float)i / 7.0f;
        Vector3 p = {
            startPos.x + sinf(t * 3.14159f) * sway,
            startPos.y - t * length,
            startPos.z + cosf(t * 2.3f) * 0.60f
        };

        DrawCylinderEx(
            { p.x, p.y + 0.75f, p.z },
            { p.x + 0.14f, p.y - 0.75f, p.z + 0.10f },
            0.12f,
            0.08f,
            6,
            DARKGREEN
        );

        DrawSphere({ p.x + 0.34f, p.y, p.z }, 0.21f, GREEN);
        DrawSphere({ p.x - 0.28f, p.y - 0.08f, p.z }, 0.18f, GREEN);
        DrawSphere({ p.x, p.y - 0.16f, p.z + 0.22f }, 0.15f, LIME);
    }
}

void drawWillowLantern(Vector3 pos) {
    Color wood = { 95, 70, 46, 255 };
    Color glowOuter = { 255, 210, 150, 120 };
    Color glowMid = { 255, 225, 170, 190 };
    Color glowInner = { 255, 245, 200, 255 };
    Color frame = { 110, 82, 55, 255 };

    // much longer hanging string
    DrawCylinderEx(
        { pos.x, pos.y + 3.2f, pos.z },
        { pos.x, pos.y + 0.2f, pos.z },
        0.05f,
        0.05f,
        5,
        wood
    );

    // lantern frame
    DrawCubeV({ pos.x, pos.y - 0.55f, pos.z }, { 1.45f, 1.05f, 1.45f }, frame);
    DrawCubeWiresV({ pos.x, pos.y - 0.55f, pos.z }, { 1.45f, 1.05f, 1.45f }, BLACK);

    // glow
    DrawCubeV({ pos.x, pos.y - 0.55f, pos.z }, { 2.8f, 2.2f, 2.8f }, glowOuter);
    DrawCubeV({ pos.x, pos.y - 0.55f, pos.z }, { 2.0f, 1.55f, 2.0f }, glowMid);
    DrawCubeV({ pos.x, pos.y - 0.55f, pos.z }, { 1.00f, 0.75f, 1.00f }, glowInner);

    // top cap
    DrawCubeV({ pos.x, pos.y + 0.2f, pos.z }, { 0.85f, 0.14f, 0.85f }, wood);
}

void drawLilyPad(Vector3 pos, float scale, Color flowerColor, bool hasFlower) {
    Color pad = { 98, 150, 88, 255 };
    Color padDark = { 78, 122, 70, 255 };

    DrawSphere({ pos.x, pos.y, pos.z }, 1.1f * scale, pad);
    DrawSphere({ pos.x - 0.25f * scale, pos.y + 0.03f, pos.z + 0.15f * scale }, 0.85f * scale, pad);
    DrawCubeV({ pos.x + 0.55f * scale, pos.y + 0.04f, pos.z }, { 0.6f * scale, 0.12f, 0.28f * scale }, padDark);

    if (hasFlower) {
        DrawCylinderEx(
            { pos.x, pos.y + 0.15f, pos.z },
            { pos.x, pos.y + 0.75f, pos.z },
            0.05f,
            0.04f,
            5,
            DARKGREEN
        );

        DrawSphere({ pos.x, pos.y + 0.88f, pos.z }, 0.22f * scale, YELLOW);
        DrawSphere({ pos.x + 0.20f * scale, pos.y + 0.84f, pos.z }, 0.18f * scale, flowerColor);
        DrawSphere({ pos.x - 0.20f * scale, pos.y + 0.84f, pos.z }, 0.18f * scale, flowerColor);
        DrawSphere({ pos.x, pos.y + 0.84f, pos.z + 0.20f * scale }, 0.18f * scale, flowerColor);
        DrawSphere({ pos.x, pos.y + 0.84f, pos.z - 0.20f * scale }, 0.18f * scale, flowerColor);
    }
}

void drawPondFlower(Vector3 basePos, Color flowerColor) {
    DrawCylinderEx(
        { basePos.x, basePos.y, basePos.z },
        { basePos.x, basePos.y + 2.2f, basePos.z },
        0.07f,
        0.05f,
        5,
        DARKGREEN
    );

    DrawSphere({ basePos.x, basePos.y + 2.45f, basePos.z }, 0.22f, YELLOW);
    DrawSphere({ basePos.x + 0.23f, basePos.y + 2.35f, basePos.z }, 0.18f, flowerColor);
    DrawSphere({ basePos.x - 0.23f, basePos.y + 2.35f, basePos.z }, 0.18f, flowerColor);
    DrawSphere({ basePos.x, basePos.y + 2.35f, basePos.z + 0.23f }, 0.18f, flowerColor);
    DrawSphere({ basePos.x, basePos.y + 2.35f, basePos.z - 0.23f }, 0.18f, flowerColor);
}

void drawDuck(Vector3 pos) {
    Color body = { 245, 235, 205, 255 };
    Color beak = ORANGE;
    Color wing = { 220, 205, 170, 255 };

    DrawSphere({ pos.x, pos.y, pos.z }, 1.0f, body);
    DrawSphere({ pos.x + 0.95f, pos.y + 0.45f, pos.z }, 0.55f, body);
    DrawSphere({ pos.x - 0.2f, pos.y + 0.15f, pos.z + 0.55f }, 0.42f, wing);

    DrawCubeV({ pos.x + 1.45f, pos.y + 0.35f, pos.z }, { 0.28f, 0.18f, 0.42f }, beak);
    DrawSphere({ pos.x + 1.1f, pos.y + 0.52f, pos.z + 0.18f }, 0.06f, BLACK);
}

void drawFirefly(Vector3 pos) {
    Color glowOuter = { 220, 255, 150, 130 };
    Color glowMid = { 235, 255, 170, 210 };
    Color glowInner = { 255, 255, 210, 255 };

    DrawSphere(pos, 0.65f, glowOuter);
    DrawSphere(pos, 0.30f, glowMid);
    DrawSphere(pos, 0.12f, glowInner);
}

void drawReedCluster(Vector3 basePos, int count) {
    for (int i = 0; i < count; i++) {
        float xOffset = ((i % 3) - 1) * 0.32f;
        float zOffset = ((i / 3) - 1) * 0.26f;
        float height = 3.4f + (i % 4) * 0.7f;

        DrawCylinderEx(
            { basePos.x + xOffset, basePos.y, basePos.z + zOffset },
            { basePos.x + xOffset, basePos.y + height, basePos.z + zOffset },
            0.07f,
            0.05f,
            5,
            DARKGREEN
        );
    }
}

void drawFlowerPatch(Vector3 pos, Color c1, Color c2, Color c3) {
    DrawSphere({ pos.x, pos.y, pos.z }, 0.55f, c1);
    DrawSphere({ pos.x + 0.9f, pos.y, pos.z + 0.3f }, 0.34f, c2);
    DrawSphere({ pos.x - 0.8f, pos.y, pos.z - 0.2f }, 0.30f, c3);
    DrawSphere({ pos.x + 0.2f, pos.y, pos.z - 0.8f }, 0.24f, YELLOW);
}

void drawWoodBorderLong(Vector3 center, Vector3 size) {
    Color wood = { 118, 86, 56, 255 };
    Color bark = { 86, 60, 38, 255 };

    DrawCubeV(center, size, wood);
    DrawCubeWiresV(center, size, bark);

    DrawCubeV(
        { center.x, center.y + size.y * 0.28f, center.z },
        { size.x * 0.96f, size.y * 0.18f, size.z * 0.96f },
        { 145, 108, 72, 255 }
    );
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug Level 2");

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
    float computerRot = 180;

    Model bugModel = LoadModel("objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSize[3] = {
        bugBounds.max.x - bugBounds.min.x,
        bugBounds.max.y - bugBounds.min.y,
        bugBounds.max.z - bugBounds.min.z
    };

    Vector3 bugPos = { -38.0f, (-42.0f + 1.0f + bugSize[1] / 2.0f), 18.0f };
    float bugRot = 0;
    float bugSpeed = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrt(0.5f * bugSpeed * bugSpeed);

    vector<Platform> platforms = {
        {{ -36.0f, -42.0f,  18.0f }, { 14.0f, 2.0f, 18.0f }, { 105, 118, 112, 255 }},

        {{ -20.0f, -38.0f,  12.0f }, { 10.0f, 2.0f, 6.0f }, { 128, 114, 96, 255 }},
        {{  -8.0f, -34.0f,   8.0f }, { 10.0f, 2.0f, 6.0f }, { 128, 114, 96, 255 }},
        {{   4.0f, -30.0f,   4.0f }, { 10.0f, 2.0f, 6.0f }, { 128, 114, 96, 255 }},
        {{  16.0f, -26.0f,   0.0f }, { 10.0f, 2.0f, 6.0f }, { 128, 114, 96, 255 }},

        {{  30.0f, -22.0f,  -2.0f }, { 16.0f, 2.0f, 18.0f }, { 105, 118, 112, 255 }},

        {{ -10.0f, -46.0f, -14.0f }, { 5.0f, 1.0f, 5.0f }, { 112, 142, 104, 255 }},
        {{   2.0f, -46.0f, -18.0f }, { 4.0f, 1.0f, 4.0f }, { 112, 142, 104, 255 }},
        {{  14.0f, -46.0f, -14.0f }, { 5.0f, 1.0f, 5.0f }, { 112, 142, 104, 255 }}
    };

    Vector3 computerPos = {
        30.0f,
        -22.0f + 1.0f + computerSize[1] / 2.0f,
        3.0f
    };

    Vector3 doorPos = { 30.0f, -17.0f, -9.0f };

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

        boundPos(currCamPos, (float[2]){-45,45}, (float[2]){-45,45}, (float[2]){-45,80});

        camera.position = currCamPos;
        camera.target = bugPos;

        float time = (float)GetTime();

        BeginDrawing();
        ClearBackground({ 205, 228, 255, 255 });

        BeginMode3D(camera);

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, Fade(BLACK, 0.28f));
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, Fade(RED, 0.15f));

        // grassy cove floor
        DrawPlane({0,-50,0}, {100, 100}, (Color){126, 176, 124, 255});
        DrawPlane({0,-50,75}, {100, 50}, (Color){132, 182, 130, 255});

        // soft glowing accent patches on the ground
        DrawCubeV({ -24.0f, -49.75f, -26.0f }, { 16.0f, 0.18f, 12.0f }, Fade(PINK, 0.12f));
        DrawCubeV({  22.0f, -49.75f,  22.0f }, { 18.0f, 0.18f, 14.0f }, Fade(PURPLE, 0.10f));
        DrawCubeV({   2.0f, -49.75f, -34.0f }, { 20.0f, 0.18f, 10.0f }, Fade(YELLOW, 0.08f));
        DrawCubeV({ -10.0f, -49.75f,  30.0f }, { 14.0f, 0.18f, 10.0f }, Fade(SKYBLUE, 0.07f));

        // pond water
        DrawCubeV({0.0f, -47.5f, 0.0f}, {70.0f, 1.0f, 55.0f}, (Color){88, 150, 205, 220});
        DrawCubeV({0.0f, -47.4f, 0.0f}, {60.0f, 0.35f, 46.0f}, Fade(SKYBLUE, 0.22f));
        DrawCubeV({0.0f, -47.3f, 0.0f}, {46.0f, 0.20f, 32.0f}, Fade(PINK, 0.05f));
        DrawCubeWiresV({0.0f, -47.5f, 0.0f}, {70.0f, 1.0f, 55.0f}, DARKBLUE);

        // wooden pond border
        drawWoodBorderLong({0.0f, -47.0f,  28.0f}, {74.0f, 2.2f, 3.4f});
        drawWoodBorderLong({0.0f, -47.0f, -28.0f}, {74.0f, 2.2f, 3.4f});
        drawWoodBorderLong({-36.0f, -47.0f, 0.0f}, {3.4f, 2.2f, 56.0f});
        drawWoodBorderLong({ 36.0f, -47.0f, 0.0f}, {3.4f, 2.2f, 56.0f});

        // lily pads and floating flowers
        drawLilyPad({ -18.0f, -46.85f,   8.0f }, 1.0f, PINK, true);
        drawLilyPad({ -10.0f, -46.85f,  -6.0f }, 0.95f, PURPLE, true);
        drawLilyPad({   0.0f, -46.85f,  12.0f }, 1.15f, PINK, true);
        drawLilyPad({   9.0f, -46.85f,  -8.0f }, 0.9f, PURPLE, false);
        drawLilyPad({  18.0f, -46.85f,   6.0f }, 1.0f, PINK, true);
        drawLilyPad({  24.0f, -46.85f, -12.0f }, 0.85f, PURPLE, true);
        drawLilyPad({ -25.0f, -46.85f, -14.0f }, 0.80f, PINK, false);
        drawLilyPad({  12.0f, -46.85f,  16.0f }, 0.90f, PURPLE, true);

        // pond flowers rising out of the water
        drawPondFlower({ -28.0f, -46.8f, 18.0f }, PINK);
        drawPondFlower({ -24.0f, -46.8f, 14.0f }, PURPLE);
        drawPondFlower({  26.0f, -46.8f, -16.0f }, PINK);
        drawPondFlower({  21.0f, -46.8f, -20.0f }, PURPLE);
        drawPondFlower({  -6.0f, -46.8f, 20.0f }, MAGENTA);
        drawPondFlower({   8.0f, -46.8f, -18.0f }, PINK);

        // reeds / grasses by pond
        drawReedCluster({ -30.0f, -47.0f, 22.0f }, 6);
        drawReedCluster({ -26.0f, -47.0f, -22.0f }, 7);
        drawReedCluster({  30.0f, -47.0f, 18.0f }, 6);
        drawReedCluster({  24.0f, -47.0f, -21.0f }, 7);
        drawReedCluster({  -8.0f, -47.0f, 24.0f }, 5);
        drawReedCluster({  10.0f, -47.0f, -24.0f }, 5);

        // ducks
        drawDuck({ -14.0f, -46.55f, 2.0f });
        drawDuck({ 12.0f, -46.55f, -4.0f });
        drawDuck({ 4.0f, -46.55f, 10.0f });

        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);
        }

        // subtle mossy glow on bridge/platform tops
        DrawCubeV({ -36.0f, -40.9f, 18.0f }, { 12.0f, 0.16f, 16.0f }, Fade(LIME, 0.10f));
        DrawCubeV({ -20.0f, -36.9f, 12.0f }, {  8.0f, 0.14f,  4.5f }, Fade(LIME, 0.08f));
        DrawCubeV({  -8.0f, -32.9f,  8.0f }, {  8.0f, 0.14f,  4.5f }, Fade(LIME, 0.08f));
        DrawCubeV({   4.0f, -28.9f,  4.0f }, {  8.0f, 0.14f,  4.5f }, Fade(LIME, 0.08f));
        DrawCubeV({  16.0f, -24.9f,  0.0f }, {  8.0f, 0.14f,  4.5f }, Fade(LIME, 0.08f));
        DrawCubeV({  30.0f, -20.9f, -2.0f }, { 14.0f, 0.16f, 16.0f }, Fade(LIME, 0.10f));

        // giant willow tree
        // trunk now extended to visually meet the floor
        Vector3 trunkPos = { 1.5f, -48.0f, -20.0f };
        DrawCylinder(trunkPos, 4.0f, 4.8f, 72.0f, 14, { 108, 76, 56, 255 });

        // root flare to anchor it into the ground
        DrawSphere({  0.0f, -49.0f, -20.5f }, 6.5f, { 108, 76, 56, 255 });
        DrawSphere({  5.2f, -48.4f, -18.0f }, 4.4f, { 108, 76, 56, 255 });
        DrawSphere({ -4.8f, -48.6f, -22.8f }, 4.2f, { 108, 76, 56, 255 });

        // larger, taller canopy
        DrawSphere({   0.0f,   4.0f, -20.0f }, 14.0f, (Color){ 98, 148, 96, 255 });
        DrawSphere({ -11.5f,   1.0f, -18.0f }, 12.0f, (Color){ 108, 160, 104, 255 });
        DrawSphere({  12.0f,   1.0f, -21.0f }, 12.3f, (Color){ 105, 155, 100, 255 });
        DrawSphere({   1.0f,   1.8f, -31.0f }, 12.5f, (Color){ 102, 152, 98, 255 });
        DrawSphere({   4.0f,   2.0f,  -8.0f }, 10.8f, (Color){ 108, 158, 102, 255 });
        DrawSphere({ -16.0f,  -1.5f, -29.0f }, 10.2f, (Color){ 104, 154, 100, 255 });
        DrawSphere({  17.0f,  -1.2f, -11.0f }, 10.0f, (Color){ 104, 154, 100, 255 });
        DrawSphere({  -6.0f,   6.0f, -27.0f },  9.8f, (Color){ 100, 150, 96, 255 });
        DrawSphere({   8.0f,   6.5f, -14.0f },  9.2f, (Color){ 100, 150, 96, 255 });

        // lower drooping leaf curtain
        DrawSphere({ -8.0f, -7.5f, -12.0f }, 7.6f, (Color){ 96, 146, 92, 240 });
        DrawSphere({  8.0f, -8.0f, -13.0f }, 7.9f, (Color){ 96, 146, 92, 240 });
        DrawSphere({  0.0f, -8.8f, -26.0f }, 8.0f, (Color){ 96, 146, 92, 240 });
        DrawSphere({ -14.5f, -10.0f, -20.0f }, 6.8f, (Color){ 96, 146, 92, 230 });
        DrawSphere({  14.5f, -10.0f, -20.0f }, 6.8f, (Color){ 96, 146, 92, 230 });

        // willow vines
        drawVine({ -14.0f, -4.0f, -16.0f }, 20.0f, 0.9f);
        drawVine({  -9.0f, -3.5f, -10.0f }, 18.5f, 0.8f);
        drawVine({  -4.0f, -3.0f, -14.0f }, 19.5f, 0.9f);
        drawVine({   1.0f, -3.0f, -26.0f }, 21.0f, 1.0f);
        drawVine({   6.0f, -3.5f, -12.0f }, 18.5f, 0.9f);
        drawVine({  11.0f, -3.0f, -18.0f }, 19.0f, 0.8f);
        drawVine({  16.0f, -4.0f, -24.0f }, 17.0f, 0.7f);
        drawVine({  -2.0f, -2.0f, -32.0f }, 16.5f, 0.8f);
        drawVine({   4.5f, -2.5f,  -7.5f }, 15.5f, 0.7f);

        // lanterns lowered so they clearly dangle
        drawWillowLantern({ -10.0f, -14.0f, -14.0f });
        drawWillowLantern({  -4.0f, -13.5f, -20.0f });
        drawWillowLantern({   2.0f, -13.8f, -26.0f });
        drawWillowLantern({   8.0f, -14.0f, -15.0f });
        drawWillowLantern({  13.0f, -14.5f, -22.0f });
        drawWillowLantern({   0.0f, -12.8f, -10.0f });

        // flower patches
        drawFlowerPatch({ -4.0f, -49.0f, -26.0f }, PINK, PURPLE, MAGENTA);
        drawFlowerPatch({  7.0f, -49.0f, -25.0f }, PURPLE, PINK, VIOLET);
        drawFlowerPatch({  1.5f, -49.0f, -11.0f }, MAGENTA, PINK, PURPLE);
        drawFlowerPatch({ -34.0f, -49.0f, 26.0f }, PINK, PURPLE, YELLOW);
        drawFlowerPatch({  34.0f, -49.0f, -24.0f }, PINK, YELLOW, PURPLE);
        drawFlowerPatch({ -22.0f, -49.0f, -30.0f }, PURPLE, PINK, MAGENTA);
        drawFlowerPatch({  20.0f, -49.0f,  28.0f }, PINK, PURPLE, YELLOW);

        // more prominent fireflies
        drawFirefly({ -4.0f + sinf(time * 1.1f) * 0.8f, -18.0f + cosf(time * 1.8f) * 0.5f, -12.0f });
        drawFirefly({  5.5f + cosf(time * 0.9f) * 1.0f, -19.0f + sinf(time * 1.5f) * 0.6f, -17.0f });
        drawFirefly({ 10.0f + sinf(time * 1.4f) * 0.9f, -21.0f + cosf(time * 2.0f) * 0.4f,  -6.0f });
        drawFirefly({ -18.0f + cosf(time * 1.0f) * 0.7f, -24.0f + sinf(time * 1.6f) * 0.4f, 10.0f });
        drawFirefly({ 22.0f + sinf(time * 1.2f) * 0.7f, -23.0f + cosf(time * 1.7f) * 0.5f, -8.0f });
        drawFirefly({  1.0f + cosf(time * 0.8f) * 0.9f, -16.0f + sinf(time * 1.3f) * 0.5f, -24.0f });
        drawFirefly({ -11.0f + sinf(time * 1.5f) * 0.8f,  -6.0f + cosf(time * 1.9f) * 0.4f, -20.0f });
        drawFirefly({  9.0f + cosf(time * 1.1f) * 0.8f,   -8.0f + sinf(time * 1.6f) * 0.4f, -22.0f });
        drawFirefly({  0.0f + sinf(time * 0.7f) * 1.0f,  -10.0f + cosf(time * 1.4f) * 0.5f, -14.0f });
        drawFirefly({ -24.0f + cosf(time * 0.9f) * 1.1f, -14.0f + sinf(time * 1.7f) * 0.6f,  8.0f });
        drawFirefly({  18.0f + sinf(time * 1.0f) * 0.9f, -12.5f + cosf(time * 1.2f) * 0.7f,  6.0f });

        DrawModelEx(
            bugModel,
            bugPos,
            (Vector3){ 0.0f, 1.0f, 0.0f },
            bugRot,
            (Vector3){ 1.0f, 1.0f, 1.0f },
            BROWN
        );

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

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, { 110, 180, 245, 255 });
        DrawCubeWiresV(doorPos, { 4.0f, 8.0f, 1.5f }, BLACK);

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
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

void drawRotatedBox(Vector3 position, Vector3 size, float rotationDegrees,
                    Color fillColor, Color wireColor) {
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotationDegrees, 0.0f, 1.0f, 0.0f);

    DrawCubeV({ 0.0f, 0.0f, 0.0f }, size, fillColor);
    DrawCubeWiresV({ 0.0f, 0.0f, 0.0f }, size, wireColor);

    rlPopMatrix();
}

void drawCloud(Vector3 center) {
    Color cloudColor = { 245, 250, 255, 235 };

    DrawSphere({ center.x, center.y, center.z }, 4.0f, cloudColor);
    DrawSphere({ center.x - 3.5f, center.y - 0.4f, center.z + 0.6f }, 3.0f, cloudColor);
    DrawSphere({ center.x + 3.6f, center.y - 0.2f, center.z + 0.4f }, 3.2f, cloudColor);
    DrawSphere({ center.x - 1.2f, center.y + 1.2f, center.z }, 2.8f, cloudColor);
    DrawSphere({ center.x + 1.8f, center.y + 1.0f, center.z - 0.3f }, 2.6f, cloudColor);
}

void drawWinterTree(Vector3 trunkPos) {
    Color trunkColor = { 105, 78, 60, 255 };
    Color leafColor = { 118, 150, 132, 255 };
    Color snowColor = { 235, 245, 255, 255 };

    DrawCubeV(trunkPos, { 2.0f, 10.0f, 2.0f }, trunkColor);
    DrawCubeWiresV(trunkPos, { 2.0f, 10.0f, 2.0f }, BLACK);

    DrawCubeV({ trunkPos.x, trunkPos.y + 5.0f, trunkPos.z }, { 10.0f, 2.5f, 10.0f }, leafColor);
    DrawCubeWiresV({ trunkPos.x, trunkPos.y + 5.0f, trunkPos.z }, { 10.0f, 2.5f, 10.0f }, BLACK);

    DrawCubeV({ trunkPos.x, trunkPos.y + 8.0f, trunkPos.z }, { 7.5f, 2.3f, 7.5f }, leafColor);
    DrawCubeWiresV({ trunkPos.x, trunkPos.y + 8.0f, trunkPos.z }, { 7.5f, 2.3f, 7.5f }, BLACK);

    DrawCubeV({ trunkPos.x, trunkPos.y + 10.5f, trunkPos.z }, { 5.0f, 2.0f, 5.0f }, leafColor);
    DrawCubeWiresV({ trunkPos.x, trunkPos.y + 10.5f, trunkPos.z }, { 5.0f, 2.0f, 5.0f }, BLACK);

    DrawCubeV({ trunkPos.x, trunkPos.y + 6.2f, trunkPos.z }, { 8.6f, 0.45f, 8.6f }, snowColor);
    DrawCubeV({ trunkPos.x, trunkPos.y + 9.1f, trunkPos.z }, { 6.4f, 0.40f, 6.4f }, snowColor);
    DrawCubeV({ trunkPos.x, trunkPos.y + 11.35f, trunkPos.z }, { 4.2f, 0.35f, 4.2f }, snowColor);
}

void drawSnowLantern(Vector3 pos) {
    Color postColor = { 95, 85, 100, 255 };
    Color glowOuter = { 190, 220, 255, 120 };
    Color glowInner = { 235, 245, 255, 255 };

    DrawCubeV({ pos.x, pos.y + 2.0f, pos.z }, { 0.35f, 4.0f, 0.35f }, postColor);
    DrawCubeWiresV({ pos.x, pos.y + 2.0f, pos.z }, { 0.35f, 4.0f, 0.35f }, BLACK);

    DrawSphere({ pos.x, pos.y + 4.5f, pos.z }, 1.2f, glowOuter);
    DrawSphere({ pos.x, pos.y + 4.5f, pos.z }, 0.55f, glowInner);
}

void drawSnowMound(Vector3 pos, float radius, Color color) {
    DrawSphere(pos, radius, color);
}

void drawMoon(Vector3 pos) {
    Color moonGlow = { 210, 230, 255, 90 };
    Color moonCore = { 245, 248, 255, 255 };

    DrawSphere(pos, 5.8f, moonGlow);
    DrawSphere(pos, 3.2f, moonCore);

    DrawSphere({ pos.x - 0.9f, pos.y + 0.6f, pos.z + 0.4f }, 0.45f, { 220, 228, 240, 255 });
    DrawSphere({ pos.x + 1.0f, pos.y - 0.7f, pos.z + 0.2f }, 0.35f, { 220, 228, 240, 255 });
    DrawSphere({ pos.x - 0.2f, pos.y - 1.1f, pos.z + 0.3f }, 0.28f, { 220, 228, 240, 255 });
}


void drawCrystalCluster(Vector3 basePos) {
    Color crystalOuter = { 160, 220, 255, 180 };
    Color crystalInner = { 220, 245, 255, 235 };

    drawRotatedBox(
        { basePos.x, basePos.y + 2.8f, basePos.z },
        { 1.3f, 5.6f, 1.3f },
        18.0f,
        crystalOuter,
        BLACK
    );

    drawRotatedBox(
        { basePos.x - 1.4f, basePos.y + 2.0f, basePos.z + 0.5f },
        { 1.0f, 4.2f, 1.0f },
        -12.0f,
        crystalOuter,
        BLACK
    );

    drawRotatedBox(
        { basePos.x + 1.2f, basePos.y + 1.7f, basePos.z - 0.4f },
        { 0.9f, 3.6f, 0.9f },
        10.0f,
        crystalOuter,
        BLACK
    );

    DrawSphere({ basePos.x, basePos.y + 2.4f, basePos.z }, 0.45f, crystalInner);
    DrawSphere({ basePos.x - 1.4f, basePos.y + 1.7f, basePos.z + 0.5f }, 0.30f, crystalInner);
    DrawSphere({ basePos.x + 1.2f, basePos.y + 1.4f, basePos.z - 0.4f }, 0.25f, crystalInner);
}


int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug Level 3");

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
    float bugRot = 0;
    float bugSpeed = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrt(0.5f * bugSpeed * bugSpeed);

    vector<Platform> platforms = {
        {{ -30.0f, -42.0f, -24.0f }, { 18.0f, 2.0f, 10.0f }, { 170, 176, 186, 255 }},
        {{ -10.0f, -34.0f, -12.0f }, { 18.0f, 2.0f, 10.0f }, { 170, 176, 186, 255 }},
        {{  27.0f, -34.0f,  18.0f }, { 16.0f, 2.0f, 16.0f }, { 170, 176, 186, 255 }}
    };

    Vector3 bugPos = { -34.0f, (-42.0f + 1.0f + bugSize[1] / 2.0f), -18.0f };

    Vector3 computerPos = {
        -16.0f,
        -34.0f + 1.0f + computerSize[1] / 2.0f,
        -15.0f
    };

    Vector3 doorPos = { 33.0f, -28.0f, 12.0f };

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

        float time = (float)GetTime();
        float snowGlow = 0.10f + 0.04f * sinf(time * 1.6f);

        BeginDrawing();
        ClearBackground({ 215, 232, 255, 255 });

        BeginMode3D(camera);

        drawMoon({ 30.0f, 16.0f, -42.0f });

        drawCloud({ -30.0f, -2.0f, -34.0f });
        drawCloud({   0.0f,  2.0f, -26.0f });
        drawCloud({  28.0f, -1.0f, -38.0f });

        DrawCubeWiresV({ 0, 0, 0 }, { 100, 100, 100 }, BLACK);
        DrawCubeWiresV({ 0, 0, 0 }, { 200, 200, 200 }, Fade(RED, 0.30f));

        DrawPlane({ 0, -50, 0 }, { 100, 100 }, { 224, 234, 242, 255 });
        DrawPlane({ 0, -50, 75 }, { 100, 50 }, { 224, 234, 242, 255 });

        DrawCubeV({ -20.0f, -49.8f, -12.0f }, { 22.0f, 0.15f, 16.0f }, Fade(SKYBLUE, 0.22f));
        DrawCubeV({ 12.0f, -49.8f, 10.0f }, { 26.0f, 0.15f, 18.0f }, Fade({ 220, 240, 255, 255 }, 0.20f));
        DrawCubeV({ 0.0f, -49.8f, 28.0f }, { 18.0f, 0.15f, 12.0f }, Fade({ 200, 230, 255, 255 }, 0.16f));

        drawSnowMound({ -42.0f, -47.0f, -36.0f }, 6.0f, { 230, 238, 245, 255 });
        drawSnowMound({ -34.0f, -47.5f, 28.0f }, 4.8f, { 230, 238, 245, 255 });
        drawSnowMound({ 36.0f, -47.0f, 30.0f }, 6.5f, { 230, 238, 245, 255 });
        drawSnowMound({ 42.0f, -47.0f, -18.0f }, 5.5f, { 230, 238, 245, 255 });
        drawSnowMound({ -6.0f, -47.8f, 38.0f }, 4.2f, { 230, 238, 245, 255 });

        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);

            DrawCubeV(
                { platform.position.x, platform.position.y + 1.05f, platform.position.z },
                { platform.size.x * 0.88f, 0.25f, platform.size.z * 0.88f },
                { 240, 246, 252, 255 }
            );
        }

        drawRotatedBox(
            { 9.0f, -31.5f, 1.5f },
            { 38.0f, 1.0f, 7.0f },
            -32.0f,
            { 165, 205, 235, 255 },
            BLACK
        );

        drawRotatedBox(
            { 10.5f, -29.8f, -1.8f },
            { 38.0f, 1.6f, 0.8f },
            -32.0f,
            { 195, 205, 220, 255 },
            BLACK
        );

        drawRotatedBox(
            { 7.5f, -29.8f, 4.8f },
            { 38.0f, 1.6f, 0.8f },
            -32.0f,
            { 195, 205, 220, 255 },
            BLACK
        );

        drawRotatedBox(
            { 9.0f, -30.9f, 1.5f },
            { 35.5f, 0.20f, 5.7f },
            -32.0f,
            { 240, 246, 252, 255 },
            { 220, 230, 240, 255 }
        );

        drawWinterTree({ -43.0f, -45.0f, -8.0f });
        drawWinterTree({ -25.0f, -45.0f, 34.0f });
        drawWinterTree({ 16.0f, -45.0f, -34.0f });
        drawWinterTree({ 42.0f, -45.0f, 0.0f });
        drawWinterTree({ 40.0f, -45.0f, 36.0f });

        drawSnowLantern({ -19.0f, -42.0f, -27.0f });
        drawSnowLantern({ 2.0f, -42.0f, 21.0f });
        drawSnowLantern({ 27.0f, -34.0f, 31.0f });

        DrawSphere({ -8.0f, -25.0f, -6.0f }, 1.3f, Fade({ 200, 230, 255, 255 }, 0.35f + snowGlow));
        DrawSphere({ 12.0f, -21.0f, 8.0f }, 1.0f, Fade({ 220, 240, 255, 255 }, 0.32f + snowGlow));
        DrawSphere({ 24.0f, -17.0f, 20.0f }, 0.9f, Fade({ 210, 235, 255, 255 }, 0.30f + snowGlow));
        DrawSphere({ -28.0f, -18.0f, 12.0f }, 1.1f, Fade({ 220, 240, 255, 255 }, 0.30f + snowGlow));

        DrawSphere({ -36.0f, -46.5f, -12.0f }, 1.6f, { 240, 246, 252, 255 });
        DrawSphere({ -34.0f, -46.3f, -10.4f }, 1.1f, { 240, 246, 252, 255 });
        DrawSphere({ 20.0f, -46.8f, 26.0f }, 1.4f, { 240, 246, 252, 255 });
        DrawSphere({ 21.4f, -46.4f, 24.8f }, 0.9f, { 240, 246, 252, 255 });

        drawCrystalCluster({ -38.0f, -49.0f,  10.0f });
        drawCrystalCluster({ -6.0f,  -49.0f,  30.0f });
        drawCrystalCluster({ 16.0f,  -49.0f, -26.0f });
        drawCrystalCluster({ 34.0f,  -49.0f,   4.0f });

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

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, { 120, 180, 255, 255 });
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
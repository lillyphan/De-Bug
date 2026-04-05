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

void drawPottedPlant(Vector3 basePos, Color leafColor, Color flowerColor) {
    Color potColor = { 150, 104, 72, 255 };

    DrawCubeV({ basePos.x, basePos.y + 0.9f, basePos.z }, { 2.4f, 1.8f, 2.4f }, potColor);
    DrawCubeWiresV({ basePos.x, basePos.y + 0.9f, basePos.z }, { 2.4f, 1.8f, 2.4f }, BLACK);

    DrawCylinderEx(
        { basePos.x, basePos.y + 1.5f, basePos.z },
        { basePos.x, basePos.y + 4.6f, basePos.z },
        0.16f,
        0.12f,
        6,
        DARKGREEN
    );

    DrawSphere({ basePos.x, basePos.y + 4.8f, basePos.z }, 1.0f, leafColor);
    DrawSphere({ basePos.x - 0.8f, basePos.y + 4.4f, basePos.z + 0.2f }, 0.75f, leafColor);
    DrawSphere({ basePos.x + 0.8f, basePos.y + 4.35f, basePos.z - 0.1f }, 0.75f, leafColor);
    DrawSphere({ basePos.x + 0.1f, basePos.y + 4.2f, basePos.z - 0.9f }, 0.7f, leafColor);

    DrawSphere({ basePos.x, basePos.y + 5.3f, basePos.z }, 0.23f, flowerColor);
    DrawSphere({ basePos.x - 0.7f, basePos.y + 4.85f, basePos.z + 0.1f }, 0.18f, flowerColor);
    DrawSphere({ basePos.x + 0.7f, basePos.y + 4.8f, basePos.z - 0.1f }, 0.18f, flowerColor);
}

void drawFlowerBed(Vector3 centerPos, Vector3 size) {
    Color soil = { 112, 85, 62, 255 };
    DrawCubeV(centerPos, size, soil);
    DrawCubeWiresV(centerPos, size, BLACK);

    for (int i = -2; i <= 2; i++) {
        float x = centerPos.x + i * 1.4f;
        float y = centerPos.y + size.y / 2.0f + 0.55f;

        DrawCylinderEx(
            { x, y - 0.2f, centerPos.z - 0.7f },
            { x, y + 0.7f, centerPos.z - 0.7f },
            0.06f,
            0.04f,
            5,
            DARKGREEN
        );
        DrawSphere({ x, y + 0.9f, centerPos.z - 0.7f }, 0.32f, PINK);

        DrawCylinderEx(
            { x + 0.4f, y - 0.2f, centerPos.z + 0.6f },
            { x + 0.4f, y + 0.6f, centerPos.z + 0.6f },
            0.06f,
            0.04f,
            5,
            DARKGREEN
        );
        DrawSphere({ x + 0.4f, y + 0.8f, centerPos.z + 0.6f }, 0.28f, YELLOW);
    }
}

void drawVine(Vector3 startPos, float length, float sway) {
    for (int i = 0; i < 6; i++) {
        float t = (float)i / 5.0f;
        Vector3 p = {
            startPos.x + sinf(t * 3.14159f) * sway,
            startPos.y - t * length,
            startPos.z + cosf(t * 2.2f) * 0.45f
        };

        DrawCylinderEx(
            { p.x, p.y + 0.55f, p.z },
            { p.x + 0.12f, p.y - 0.55f, p.z + 0.08f },
            0.10f,
            0.07f,
            6,
            DARKGREEN
        );

        DrawSphere({ p.x + 0.28f, p.y, p.z }, 0.18f, GREEN);
        DrawSphere({ p.x - 0.25f, p.y - 0.08f, p.z }, 0.16f, GREEN);
    }
}

void drawLantern(Vector3 pos) {
    Color wood = { 95, 70, 46, 255 };
    Color glow = { 255, 230, 175, 220 };

    DrawCubeV({ pos.x, pos.y + 0.15f, pos.z }, { 0.22f, 1.6f, 0.22f }, wood);
    DrawCubeV({ pos.x, pos.y - 0.8f, pos.z }, { 1.8f, 0.18f, 0.18f }, wood);

    DrawCubeV({ pos.x, pos.y - 1.35f, pos.z }, { 1.4f, 1.0f, 1.4f }, { 80, 65, 45, 255 });
    DrawCubeWiresV({ pos.x, pos.y - 1.35f, pos.z }, { 1.4f, 1.0f, 1.4f }, BLACK);
    DrawCubeV({ pos.x, pos.y - 1.35f, pos.z }, { 0.9f, 0.65f, 0.9f }, glow);
    DrawCubeV({ pos.x, pos.y + 0.75f, pos.z }, { 0.8f, 0.18f, 0.8f }, wood);
}

void drawHangingPlanter(Vector3 pos, Color potColor, Color leafColor, Color flowerColor) {
    DrawCylinderEx({ pos.x, pos.y + 2.8f, pos.z }, { pos.x, pos.y + 1.1f, pos.z }, 0.05f, 0.05f, 6, { 180, 155, 120, 255 });
    DrawCylinderEx({ pos.x - 0.6f, pos.y + 2.7f, pos.z + 0.2f }, { pos.x - 0.3f, pos.y + 1.15f, pos.z + 0.1f }, 0.05f, 0.05f, 6, { 180, 155, 120, 255 });
    DrawCylinderEx({ pos.x + 0.6f, pos.y + 2.7f, pos.z - 0.2f }, { pos.x + 0.3f, pos.y + 1.15f, pos.z - 0.1f }, 0.05f, 0.05f, 6, { 180, 155, 120, 255 });

    DrawCubeV({ pos.x, pos.y + 0.65f, pos.z }, { 2.0f, 1.2f, 2.0f }, potColor);
    DrawCubeWiresV({ pos.x, pos.y + 0.65f, pos.z }, { 2.0f, 1.2f, 2.0f }, BLACK);

    DrawSphere({ pos.x, pos.y + 1.55f, pos.z }, 0.85f, leafColor);
    DrawSphere({ pos.x - 0.55f, pos.y + 1.3f, pos.z + 0.15f }, 0.55f, leafColor);
    DrawSphere({ pos.x + 0.55f, pos.y + 1.3f, pos.z - 0.1f }, 0.55f, leafColor);

    DrawSphere({ pos.x, pos.y + 1.9f, pos.z }, 0.18f, flowerColor);
    DrawSphere({ pos.x - 0.45f, pos.y + 1.65f, pos.z + 0.05f }, 0.14f, flowerColor);
    DrawSphere({ pos.x + 0.45f, pos.y + 1.6f, pos.z - 0.05f }, 0.14f, flowerColor);
}

void drawArchedWindow(Vector3 centerPos, float width, float height) {
    Color frame = { 103, 140, 112, 255 };
    Color glass = Fade({ 180, 230, 205, 255 }, 0.18f);

    DrawCubeV({ centerPos.x - width / 2.0f, centerPos.y, centerPos.z }, { 0.9f, height, 0.5f }, frame);
    DrawCubeV({ centerPos.x + width / 2.0f, centerPos.y, centerPos.z }, { 0.9f, height, 0.5f }, frame);
    DrawCubeV({ centerPos.x, centerPos.y - height / 2.0f, centerPos.z }, { width, 0.9f, 0.5f }, frame);

    DrawCubeV({ centerPos.x, centerPos.y, centerPos.z }, { width - 1.6f, height - 1.6f, 0.18f }, glass);

    for (int i = -2; i <= 2; i++) {
        DrawSphere({ centerPos.x + i * (width / 6.0f), centerPos.y + height / 2.0f - 0.9f, centerPos.z }, 0.95f, frame);
    }

    DrawCubeV({ centerPos.x, centerPos.y + height / 2.0f - 0.2f, centerPos.z }, { width - 2.0f, 0.55f, 0.45f }, frame);
}

void drawStoneArchway(Vector3 basePos) {
    Color stone = { 110, 112, 118, 255 };
    Color moss = { 120, 155, 105, 255 };

    DrawCubeV({ basePos.x - 12.0f, basePos.y + 12.0f, basePos.z }, { 4.2f, 24.0f, 4.2f }, stone);
    DrawCubeWiresV({ basePos.x - 12.0f, basePos.y + 12.0f, basePos.z }, { 4.2f, 24.0f, 4.2f }, BLACK);

    DrawCubeV({ basePos.x + 12.0f, basePos.y + 12.0f, basePos.z }, { 4.2f, 24.0f, 4.2f }, stone);
    DrawCubeWiresV({ basePos.x + 12.0f, basePos.y + 12.0f, basePos.z }, { 4.2f, 24.0f, 4.2f }, BLACK);

    DrawCubeV({ basePos.x, basePos.y + 27.0f, basePos.z }, { 16.0f, 4.0f, 4.6f }, stone);
    DrawCubeWiresV({ basePos.x, basePos.y + 27.0f, basePos.z }, { 16.0f, 4.0f, 4.6f }, BLACK);

    DrawCubeV({ basePos.x - 7.5f, basePos.y + 24.0f, basePos.z }, { 6.0f, 4.0f, 4.6f }, stone);
    DrawCubeWiresV({ basePos.x - 7.5f, basePos.y + 24.0f, basePos.z }, { 6.0f, 4.0f, 4.6f }, BLACK);

    DrawCubeV({ basePos.x + 7.5f, basePos.y + 24.0f, basePos.z }, { 6.0f, 4.0f, 4.6f }, stone);
    DrawCubeWiresV({ basePos.x + 7.5f, basePos.y + 24.0f, basePos.z }, { 6.0f, 4.0f, 4.6f }, BLACK);

    rlPushMatrix();
    rlTranslatef(basePos.x - 9.8f, basePos.y + 20.5f, basePos.z);
    rlRotatef(-28.0f, 0.0f, 0.0f, 1.0f);
    DrawCubeV({ 0.0f, 0.0f, 0.0f }, { 4.0f, 10.0f, 4.2f }, stone);
    DrawCubeWiresV({ 0.0f, 0.0f, 0.0f }, { 4.0f, 10.0f, 4.2f }, BLACK);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(basePos.x + 9.8f, basePos.y + 20.5f, basePos.z);
    rlRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    DrawCubeV({ 0.0f, 0.0f, 0.0f }, { 4.0f, 10.0f, 4.2f }, stone);
    DrawCubeWiresV({ 0.0f, 0.0f, 0.0f }, { 4.0f, 10.0f, 4.2f }, BLACK);
    rlPopMatrix();

    DrawCubeV({ basePos.x, basePos.y + 29.6f, basePos.z }, { 12.0f, 0.45f, 3.8f }, moss);
    DrawCubeV({ basePos.x - 7.8f, basePos.y + 25.6f, basePos.z }, { 3.2f, 0.35f, 3.5f }, moss);
    DrawCubeV({ basePos.x + 7.8f, basePos.y + 25.6f, basePos.z }, { 3.2f, 0.35f, 3.5f }, moss);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug");

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
        {{ -30.0f, -24.0f, -16.0f }, { 20.0f, 2.0f, 12.0f }, { 108, 124, 86, 255 }},
        {{ -12.0f, -32.0f, -10.0f }, { 12.0f, 2.0f, 12.0f }, { 116, 132, 92, 255 }},
        {{   4.0f, -40.0f,  -2.0f }, { 10.0f, 2.0f, 10.0f }, { 108, 124, 86, 255 }},
        {{  22.0f, -48.0f,   8.0f }, { 18.0f, 2.0f, 14.0f }, { 120, 138, 96, 255 }},
        {{  24.0f, -34.0f,  -6.0f }, { 16.0f, 2.0f, 10.0f }, { 112, 130, 90, 255 }}
    };

    Vector3 doorPos = { 29.0f, -44.0f, 2.0f };
    Vector3 filePos = { 24.0f, -31.6f, -6.0f };

    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
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

        BeginDrawing();
        ClearBackground({ 224, 240, 221, 255 });

        BeginMode3D(camera);

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, Fade(BLACK, 0.25f));
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, Fade(RED, 0.14f));

        DrawPlane({0,-50,0}, {100, 100}, { 168, 194, 150, 255 });
        DrawPlane({0,-50,75}, {100, 50}, { 160, 186, 142, 255 });

        drawArchedWindow({ -28.0f, -14.0f, -49.0f }, 22.0f, 28.0f);
        drawArchedWindow({   0.0f, -14.0f, -49.0f }, 22.0f, 28.0f);
        drawArchedWindow({  28.0f, -14.0f, -49.0f }, 22.0f, 28.0f );

        DrawCubeV({ -49.0f, -12.0f, 0.0f }, { 1.0f, 76.0f, 92.0f }, Fade(SKYBLUE, 0.14f));
        DrawCubeWiresV({ -49.0f, -12.0f, 0.0f }, { 1.0f, 76.0f, 92.0f }, Fade(DARKGREEN, 0.28f));
        DrawCubeV({ 49.0f, -12.0f, 0.0f }, { 1.0f, 76.0f, 92.0f }, Fade(SKYBLUE, 0.14f));
        DrawCubeWiresV({ 49.0f, -12.0f, 0.0f }, { 1.0f, 76.0f, 92.0f }, Fade(DARKGREEN, 0.28f));

        // beams lowered more
        DrawCubeV({ 0.0f, 8.5f, -24.0f }, { 100.0f, 1.0f, 1.1f }, { 106, 138, 108, 255 });
        DrawCubeV({ 0.0f, 8.5f,   0.0f }, { 100.0f, 1.0f, 1.1f }, { 106, 138, 108, 255 });
        DrawCubeV({ 0.0f, 8.5f,  24.0f }, { 100.0f, 1.0f, 1.1f }, { 106, 138, 108, 255 });
        DrawCubeV({ -28.0f, 8.5f, 0.0f }, { 1.1f, 1.0f, 92.0f }, { 106, 138, 108, 255 });
        DrawCubeV({   0.0f, 8.5f, 0.0f }, { 1.1f, 1.0f, 92.0f }, { 106, 138, 108, 255 });
        DrawCubeV({  28.0f, 8.5f, 0.0f }, { 1.1f, 1.0f, 92.0f }, { 106, 138, 108, 255 });

        drawStoneArchway({ 0.0f, -50.0f, -36.0f });

        // lights lowered more
        DrawCubeV({ 0.0f, 6.5f, -12.0f }, { 36.0f, 0.3f, 0.3f }, { 140, 115, 85, 255 });
        drawLantern({ -14.0f, 6.5f, -12.0f });
        drawLantern({ 0.0f, 6.5f, -12.0f });
        drawLantern({ 14.0f, 6.5f, -12.0f });

        DrawCubeV({ 26.0f, 5.8f, 16.0f }, { 18.0f, 0.3f, 0.3f }, { 140, 115, 85, 255 });
        drawLantern({ 20.0f, 5.8f, 16.0f });
        drawLantern({ 32.0f, 5.8f, 16.0f });

        // hanging pots lowered more
        drawHangingPlanter({ -34.0f, 2.8f, -6.0f }, { 175, 88, 88, 255 }, GREEN, YELLOW);
        drawHangingPlanter({ -6.0f, 3.5f, -18.0f }, { 90, 120, 180, 255 }, LIME, PINK);
        drawHangingPlanter({ 18.0f, 3.2f, 10.0f }, { 160, 110, 70, 255 }, GREEN, ORANGE);
        drawHangingPlanter({ 38.0f, 3.5f, -8.0f }, { 175, 88, 88, 255 }, GREEN, PINK);

        // vines lowered more
        drawVine({ -42.0f, 8.5f, -18.0f }, 11.0f, 0.8f);
        drawVine({ -16.0f, 8.5f, 10.0f }, 9.0f, 0.9f);
        drawVine({ 8.0f, 8.5f, -16.0f }, 10.0f, 0.7f);
        drawVine({ 30.0f, 8.5f, 14.0f }, 11.0f, 0.8f);
        drawVine({ 44.0f, 8.5f, -4.0f }, 8.0f, 0.6f);

        DrawCubeV({ -36.0f, -32.0f, -34.0f }, { 16.0f, 0.8f, 3.0f }, { 120, 95, 70, 255 });
        DrawCubeV({ 34.0f, -30.0f, -30.0f }, { 14.0f, 0.8f, 3.0f }, { 120, 95, 70, 255 });
        drawPottedPlant({ -42.0f, -33.0f, -34.0f }, GREEN, PINK);
        drawPottedPlant({ -36.0f, -33.0f, -34.0f }, LIME, YELLOW);
        drawPottedPlant({ 28.0f, -31.0f, -30.0f }, GREEN, ORANGE);
        drawPottedPlant({ 36.0f, -31.0f, -30.0f }, LIME, PINK);

        drawFlowerBed({ -36.0f, -48.5f, -24.0f }, { 10.0f, 3.0f, 7.0f });
        drawFlowerBed({ -38.0f, -48.5f, 20.0f }, { 8.0f, 3.0f, 8.0f });
        drawFlowerBed({ 36.0f, -48.5f, -20.0f }, { 10.0f, 3.0f, 7.0f });
        drawFlowerBed({ 34.0f, -48.5f, 24.0f }, { 9.0f, 3.0f, 8.0f });

        DrawSphere({ -14.0f, -49.2f, 24.0f }, 0.6f, PINK);
        DrawSphere({ -12.5f, -49.2f, 23.2f }, 0.4f, YELLOW);
        DrawSphere({ 10.0f, -49.2f, -24.0f }, 0.6f, ORANGE);
        DrawSphere({ 12.0f, -49.2f, -23.5f }, 0.35f, PINK);
        DrawSphere({ 24.0f, -49.2f, 18.0f }, 0.55f, MAGENTA);
        DrawSphere({ -2.0f, -49.2f, -10.0f }, 0.45f, YELLOW);

        for (const Platform &platform : platforms) {
            DrawCubeV(platform.position, platform.size, platform.color);
            DrawCubeWiresV(platform.position, platform.size, BLACK);
        }

        DrawCubeV({ -30.0f, -22.8f, -16.0f }, { 14.0f, 0.18f, 8.5f }, Fade(GREEN, 0.20f));
        DrawCubeV({ -12.0f, -30.8f, -10.0f }, { 8.5f, 0.18f, 8.5f }, Fade(GREEN, 0.20f));
        DrawCubeV({ 24.0f, -32.8f, -6.0f }, { 10.5f, 0.18f, 6.5f }, Fade(GREEN, 0.20f));
        DrawCubeV({ 22.0f, -46.8f, 8.0f }, { 14.0f, 0.18f, 10.0f }, Fade(GREEN, 0.18f));

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

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, { 90, 148, 205, 255 });
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

    UnloadModel(computerModel);
    UnloadModel(computerScreenModel);
    UnloadModel(bugModel);
    CloseWindow();

    return 0;
}
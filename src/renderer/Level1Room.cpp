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

void drawFish(Vector3 pos, Color bodyColor, float facing) {
    DrawSphere(pos, 1.2f, bodyColor);
    DrawSphere({ pos.x + 0.9f * facing, pos.y + 0.15f, pos.z + 0.9f }, 0.12f, BLACK);

    rlPushMatrix();
    rlTranslatef(pos.x - 1.5f * facing, pos.y, pos.z);
    rlRotatef((facing > 0.0f) ? 25.0f : -25.0f, 0.0f, 1.0f, 0.0f);
    DrawCubeV({ 0.0f, 0.0f, 0.0f }, { 1.2f, 1.0f, 0.4f }, bodyColor);
    rlPopMatrix();

    DrawCubeV({ pos.x - 0.2f * facing, pos.y + 0.7f, pos.z }, { 0.9f, 0.25f, 0.5f }, bodyColor);
}

void drawJellyfish(Vector3 pos, Color bodyColor) {
    DrawSphere(pos, 1.3f, bodyColor);

    for (int i = -1; i <= 1; i++) {
        float x = pos.x + i * 0.5f;
        DrawCylinderEx(
            { x, pos.y - 0.6f, pos.z + 0.1f * i },
            { x + 0.2f * i, pos.y - 3.2f, pos.z + 0.15f * i },
            0.08f,
            0.05f,
            6,
            Fade(bodyColor, 0.8f)
        );
    }
}

void drawSeaweed(Vector3 basePos, Color color) {
    DrawCylinderEx(
        { basePos.x, basePos.y, basePos.z },
        { basePos.x + 0.4f, basePos.y + 4.5f, basePos.z + 0.2f },
        0.30f,
        0.18f,
        6,
        color
    );

    DrawCylinderEx(
        { basePos.x + 0.6f, basePos.y, basePos.z - 0.3f },
        { basePos.x + 1.0f, basePos.y + 3.6f, basePos.z + 0.5f },
        0.24f,
        0.14f,
        6,
        color
    );
}

void drawBubbleColumn(Vector3 basePos, float timeOffset) {
    for (int i = 0; i < 5; i++) {
        float rise = fmodf((float)GetTime() * 1.2f + timeOffset + i * 1.4f, 8.0f);
        Vector3 bubblePos = {
            basePos.x + sinf((float)GetTime() + i) * 0.15f,
            basePos.y + rise,
            basePos.z + cosf((float)GetTime() + i) * 0.15f
        };
        DrawSphere(bubblePos, 0.22f + 0.03f * i, Fade(SKYBLUE, 0.65f));
    }
}

void drawStingrayPlatform(Vector3 pos) {
    Color rayColor = { 85, 105, 120, 255 };

    DrawSphere({ pos.x, pos.y, pos.z }, 3.4f, rayColor);
    DrawSphere({ pos.x - 2.4f, pos.y, pos.z }, 2.2f, rayColor);
    DrawSphere({ pos.x + 2.4f, pos.y, pos.z }, 2.2f, rayColor);

    DrawCubeV({ pos.x, pos.y + 0.4f, pos.z }, { 9.0f, 1.2f, 8.0f }, rayColor);

    DrawCylinderEx(
        { pos.x, pos.y - 0.2f, pos.z - 4.0f },
        { pos.x, pos.y - 0.6f, pos.z - 11.0f },
        0.18f,
        0.05f,
        6,
        rayColor
    );

    DrawSphere({ pos.x - 1.0f, pos.y + 1.1f, pos.z + 1.2f }, 0.18f, BLACK);
    DrawSphere({ pos.x + 1.0f, pos.y + 1.1f, pos.z + 1.2f }, 0.18f, BLACK);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "De-Bug Level 1");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
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

    Vector3 bugPos = { 32.0f, (-40.0f + 1.0f + bugSize[1] / 2.0f), 28.0f };
    float bugRot = 0;
    float bugSpeed = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrt(0.5f * bugSpeed * bugSpeed);

    vector<Platform> platforms = {
        {{  28.0f, -40.0f,  22.0f }, { 14.0f, 2.0f, 14.0f }, { 95, 125, 135, 255 }},
        {{   8.0f, -28.0f,   2.0f }, { 14.0f, 2.0f, 14.0f }, { 90, 120, 130, 255 }},
        {{ -16.0f, -16.0f, -20.0f }, { 14.0f, 2.0f, 14.0f }, { 95, 125, 135, 255 }},
        {{  16.0f,  -4.0f, -30.0f }, { 16.0f, 2.0f, 16.0f }, { 100, 135, 145, 255 }}
    };

    Vector3 computerPos = {
        12.0f,
        -4.0f + 1.0f + computerSize[1] / 2.0f,
        -30.0f
    };

    Vector3 doorPos = { 22.0f, 1.0f, -34.0f };

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

        BeginDrawing();
        ClearBackground({ 0, 0, 0, 255 });

        BeginMode3D(camera);

        DrawCubeWiresV({0,0,0}, {100, 100, 100}, Fade(BLACK, 0.35f));
        DrawCubeWiresV({0,0,0}, {200, 200, 200}, Fade(RED, 0.25f));

        // full aquarium floor
        DrawPlane({ 0,-50,0 }, { 100, 100 }, { 214, 196, 145, 255 });
        DrawPlane({0,-50,75}, {100, 50}, { 200, 185, 140, 255 });

        // full aquarium glass walls
        DrawCubeV({ 0.0f, -10.0f, -46.0f }, { 98.0f, 80.0f, 1.2f }, Fade(BLUE, 0.32f));
        DrawCubeWiresV({ 0.0f, -10.0f, -46.0f }, { 98.0f, 80.0f, 1.2f }, Fade(DARKBLUE, 0.55f));

        DrawCubeV({ -49.0f, -10.0f, 0.0f }, { 1.2f, 80.0f, 92.0f }, Fade(BLUE, 0.24f));
        DrawCubeWiresV({ -49.0f, -10.0f, 0.0f }, { 1.2f, 80.0f, 92.0f }, Fade(DARKBLUE, 0.45f));

        DrawCubeV({ 49.0f, -10.0f, 0.0f }, { 1.2f, 80.0f, 92.0f }, Fade(BLUE, 0.24f));
        DrawCubeWiresV({ 49.0f, -10.0f, 0.0f }, { 1.2f, 80.0f, 92.0f }, Fade(DARKBLUE, 0.45f));

        // coral and rocks
        DrawSphere({ -32.0f, -48.0f, -22.0f }, 2.6f, PINK);
        DrawSphere({ -28.0f, -47.5f, -19.0f }, 2.2f, ORANGE);
        DrawSphere({ -36.0f, -47.8f, -16.0f }, 1.8f, PURPLE);
        DrawSphere({ -18.0f, -48.2f, 12.0f }, 2.4f, { 90, 100, 110, 255 });
        DrawSphere({ -13.0f, -48.5f, 15.0f }, 1.7f, { 80, 90, 100, 255 });

        // seaweed
        drawSeaweed({ -34.0f, -50.0f, -10.0f }, DARKGREEN);
        drawSeaweed({ -25.0f, -50.0f, 8.0f }, GREEN);
        drawSeaweed({ -15.0f, -50.0f, -28.0f }, DARKGREEN);
        drawSeaweed({ 8.0f, -50.0f, 14.0f }, GREEN);
        drawSeaweed({ 22.0f, -50.0f, -18.0f }, DARKGREEN);

        // bubbles
        drawBubbleColumn({ -31.0f, -48.0f, -12.0f }, 0.0f);
        drawBubbleColumn({ -21.0f, -48.0f, 9.0f }, 1.4f);
        drawBubbleColumn({ -12.0f, -48.0f, -25.0f }, 2.6f);
        drawBubbleColumn({ 10.0f, -48.0f, 16.0f }, 3.2f);
        drawBubbleColumn({ 24.0f, -48.0f, -10.0f }, 4.1f);

        // fish and creatures
        drawFish({ -28.0f, -18.0f, -24.0f }, ORANGE, 1.0f);
        drawFish({ -18.0f, -12.0f, -5.0f }, GOLD, -1.0f);s
        drawFish({ -34.0f, -6.0f, 12.0f }, SKYBLUE, 1.0f);
        drawFish({ -10.0f, -22.0f, 24.0f }, LIME, -1.0f);

        drawJellyfish({ -38.0f, -8.0f, -6.0f }, Fade(VIOLET, 0.9f));
        drawJellyfish({ -22.0f, -2.0f, 18.0f }, Fade(PINK, 0.9f));

        // turtl
        DrawSphere({ 6.0f, -18.0f, 6.0f }, 1.6f, DARKGREEN);
        DrawSphere({ 7.8f, -18.0f, 6.0f }, 0.8f, GREEN);
        DrawCubeV({ 5.0f, -18.8f, 7.2f }, { 0.8f, 0.2f, 1.4f }, GREEN);
        DrawCubeV({ 5.0f, -18.8f, 4.8f }, { 0.8f, 0.2f, 1.4f }, GREEN);
        DrawCubeV({ 6.8f, -18.8f, 7.2f }, { 0.8f, 0.2f, 1.4f }, GREEN);
        DrawCubeV({ 6.8f, -18.8f, 4.8f }, { 0.8f, 0.2f, 1.4f }, GREEN);

        // extra fish
        drawFish({ 6.0f, -10.0f, -18.0f }, RED, 1.0f);
        drawFish({ 24.0f, -14.0f, 8.0f }, BLUE, -1.0f);
        drawFish({ 30.0f, -6.0f, -28.0f }, YELLOW, 1.0f);

        // extra jellyfish
        drawJellyfish({ 12.0f, -4.0f, 18.0f }, Fade(SKYBLUE, 0.9f));
        drawJellyfish({ 28.0f, -8.0f, -6.0f }, Fade(PINK, 0.85f));

        // little school of fish
        drawFish({ -4.0f, -16.0f, 14.0f }, ORANGE, 1.0f);
        drawFish({ 0.0f, -14.5f, 16.0f }, GOLD, 1.0f);
        drawFish({ 4.0f, -13.0f, 18.0f }, SKYBLUE, 1.0f);

        // decorative crab on the sand
        DrawSphere({ 20.0f, -48.3f, -10.0f }, 1.0f, RED);
        DrawSphere({ 19.0f, -48.5f, -10.8f }, 0.25f, RED);
        DrawSphere({ 21.0f, -48.5f, -10.8f }, 0.25f, RED);
        DrawCubeV({ 18.5f, -48.6f, -9.2f }, { 0.8f, 0.15f, 1.0f }, RED);
        DrawCubeV({ 21.5f, -48.6f, -9.2f }, { 0.8f, 0.15f, 1.0f }, RED);

        for (int i = 0; i < (int)platforms.size(); i++) {
            const Platform &platform = platforms[i];

            if (i == 1) {
                drawStingrayPlatform(platform.position);
            }
            else {
                DrawCubeV(platform.position, platform.size, platform.color);
                DrawCubeWiresV(platform.position, platform.size, BLACK);
            }
        }

        DrawCubeV({ 12.0f, computerPos.y + 0.2f, -30.0f }, { 5.0f, 0.2f, 3.0f }, Fade(SKYBLUE, 0.25f));

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

        DrawCubeV(doorPos, { 4.0f, 8.0f, 1.5f }, BLUE);
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
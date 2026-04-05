#include <cmath>
#include <iostream>
#include "raylib.h"
#include "raymath.h"

// shader
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#define GLSL_VERSION 330

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
    camera.fovy = 30.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera mode type

    Vector3 camPos = { 0.0f, 10.0f, 20.0f }; // Delta pos from bug

    // Load Models
    Model computerModel = LoadModel("./src/renderer/objects/computer.obj");
    Model computerScreenModel = LoadModel("./src/renderer/objects/computer-screen.obj");
    BoundingBox computerBounds = GetMeshBoundingBox(computerModel.meshes[0]);
    float computerSize[3] = {
        computerBounds.max.x - computerBounds.min.x,
        computerBounds.max.y - computerBounds.min.y,
        computerBounds.max.z - computerBounds.min.z
    };
    Vector3 computerPos = { 0.0f, (-50.0f + computerSize[1] / 2), 0.0f };
    float computerRot = 180;

    Model bugModel = LoadModel("./src/renderer/objects/de-bug.obj");
    BoundingBox bugBounds = GetMeshBoundingBox(bugModel.meshes[0]);
    float bugSize[3] = {
        bugBounds.max.x - bugBounds.min.x,
        bugBounds.max.y - bugBounds.min.y,
        bugBounds.max.z - bugBounds.min.z
    };
    Vector3 bugPos = { 0.0f, (-50.0f + bugSize[1] / 2), 45.0f }; // Spawn point
    float bugRot = 0;
    float bugSpeed = 0.15 * (60 / (float)fps);
    float diagBugSpeed = sqrt(0.5 * bugSpeed * bugSpeed);

    // ---------------------------------------------------------------------------------
    // Shaders Setup
    // ---------------------------------------------------------------------------------
    Shader lightShader = LoadShader(
        "./src/assets/shaders/lighting.vs",
        "./src/assets/shaders/lighting.fs"
    );
    lightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightShader, "viewPos");
    int ambientLoc = GetShaderLocation(lightShader, "ambient");
    float ambient[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
    SetShaderValue(lightShader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

    Light sunLight = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 50, 50, 50 }, (Vector3){ 0, 0, 0 }, (Color){ 255, 240, 200, 255 }, lightShader);
    Light fillLight = CreateLight(LIGHT_POINT, (Vector3){ -30, 20, -30 }, (Vector3){ 0, 0, 0 }, (Color){ 60, 70, 120, 255 }, lightShader);

    // APPLY SHADER TO LOADED MODELS
    computerModel.materials[0].shader = lightShader;
    computerScreenModel.materials[0].shader = lightShader;
    bugModel.materials[0].shader = lightShader;

    // ASCII post-processing shader
    Shader asciiShader = LoadShader(0, TextFormat("./src/assets/shaders/ascii.fs", GLSL_VERSION));
    int resolutionLoc = GetShaderLocation(asciiShader, "resolution");
    int fontSizeLoc   = GetShaderLocation(asciiShader, "fontSize");
    float fontSize      = 10.0f;
    float resolution[2] = { (float)screenWidth, (float)screenHeight };

    SetShaderValue(asciiShader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(asciiShader, fontSizeLoc, &fontSize, SHADER_UNIFORM_FLOAT);

    // Render texture
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    SetTargetFPS(fps);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        bool keyW = IsKeyDown(KEY_W);
        bool keyA = IsKeyDown(KEY_A);
        bool keyS = IsKeyDown(KEY_S);
        bool keyD = IsKeyDown(KEY_D);

        float currBugSpeed = bugSpeed - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2) + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);

        // Check if moving diagonal
        if ((keyW || keyS) && (keyA || keyD)) {
            currBugSpeed = diagBugSpeed - (IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2) + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);
        }

        // Movement Logic
        if (keyW) bugPos.z -= currBugSpeed;
        if (keyA) bugPos.x -= currBugSpeed;
        if (keyS) bugPos.z += currBugSpeed;
        if (keyD) bugPos.x += currBugSpeed;

        // Rotation Logic
        if (keyW && keyA) {
            bugRot = 45;
        } else if (keyW && keyD) {
            bugRot = 315;
        } else if (keyS && keyA) {
            bugRot = 135;
        } else if (keyS && keyD) {
            bugRot = 225;
        } else if (keyW) {
            bugRot = 0;
        } else if (keyA) {
            bugRot = 90;
        } else if (keyS) {
            bugRot = 180;
        } else if (keyD) {
            bugRot = 270;
        }

        // Bound the bug
        boundPos(
            bugPos,
            (float[2]){-50 + bugSize[0]/2, 50 - bugSize[0]/2},
            (float[2]){-100 + bugSize[1]/2, 50 - bugSize[1]/2},
            (float[2]){-50 + bugSize[2]/2, 50 - bugSize[2]/2}
        );

        // Camera to follow bug
        Vector3 currCamPos = {camPos.x + bugPos.x, camPos.y + bugPos.y, camPos.z + bugPos.z};
        boundPos(currCamPos, (float[2]){-45, 45}, (float[2]){-45, 45}, (float[2]){-45, 80});

        camera.position = currCamPos;
        camera.target = bugPos;

        // Update Shader View Position
        float camPosArr[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(lightShader, lightShader.locs[SHADER_LOC_VECTOR_VIEW], camPosArr, SHADER_UNIFORM_VEC3);

        UpdateLightValues(lightShader, sunLight);
        UpdateLightValues(lightShader, fillLight);

        if (IsWindowResized()) {
            UnloadRenderTexture(target);
            target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
            float res[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };
            SetShaderValue(asciiShader, resolutionLoc, res, SHADER_UNIFORM_VEC2);
        }

        if (IsKeyPressed(KEY_LEFT) )  fontSize -= 1.0f;
        if (IsKeyPressed(KEY_RIGHT)) fontSize += 1.0f;
        SetShaderValue(asciiShader, fontSizeLoc, &fontSize, SHADER_UNIFORM_FLOAT);

        if (IsKeyPressed(KEY_UP) )  camera.fovy -= 1.0f;
        if (IsKeyPressed(KEY_DOWN)) camera.fovy += 1.0f;
        cout << camera.fovy << endl;

        // --------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);
            ClearBackground(BLACK);
            BeginMode3D(camera);

                DrawCubeWiresV({0,0,0}, {100, 100, 100}, BLACK);
                DrawCubeWiresV({0,0,0}, {200, 200, 200}, RED);

                // Draw objects with Default Raylib Shader
                DrawPlane({0,-50,0}, {100, 100}, (Color){ 0, 0, 0, 255 });
                DrawPlane({0,-50,75}, {100, 50}, GRAY);

                // Draw Models (These automatically use your lightShader now)
                DrawModelEx(bugModel, bugPos, (Vector3){ 0.0f, 1.0f, 0.0f }, bugRot, (Vector3){ 1.0f, 1.0f, 1.0f }, BROWN);
                DrawModelEx(computerModel, computerPos, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
                DrawModelEx(computerScreenModel, computerPos, (Vector3){ 0.0f, 1.0f, 0.0f }, computerRot, (Vector3){ 1.0f, 1.0f, 1.0f }, GREEN);

            EndMode3D();
        EndTextureMode();

        // --- Apply ASCII shader and draw to screen ---
        BeginDrawing();
            ClearBackground(BLACK);
            BeginShaderMode(asciiShader);
                DrawTextureRec(target.texture,
                    (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
                    (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
            DrawFPS(10, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Clean up models
    UnloadModel(bugModel);
    UnloadModel(computerModel);
    UnloadModel(computerScreenModel);

    // Clean up shaders/textures
    UnloadShader(lightShader);
    UnloadShader(asciiShader);
    UnloadRenderTexture(target);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

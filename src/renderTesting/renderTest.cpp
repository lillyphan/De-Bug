#include <cmath>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#define GLSL_VERSION 330

void boundPos(Vector3 &var, float x[2], float y[2], float z[2]) {
    if (var.x > x[1]) var.x = x[1];
    if (var.x < x[0]) var.x = x[0];
    if (var.y > y[1]) var.y = y[1];
    if (var.y < y[0]) var.y = y[0];
    if (var.z > z[1]) var.z = z[1];
    if (var.z < z[0]) var.z = z[0];
}

struct Platform {
    Vector3 position;
    Vector3 size;
    Color   color;
};

int main(void)
{
    const int fps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "De-Bug");

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // -------------------------------------------------------
    // Camera
    // -------------------------------------------------------
    Camera3D camera = { 0 };
    camera.position   = (Vector3){ 0.0f, 15.0f, 10.0f };
    camera.target     = (Vector3){ 0.0f,  0.0f,  0.0f };
    camera.up         = (Vector3){ 0.0f,  1.0f,  0.0f };
    camera.fovy       = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 bugPos     = {  0.0f, -48.0f, 45.0f };
    Vector3 camPos     = {  0.0f,  10.0f, 20.0f };
    float bugSpeed     = 0.15f * (60.0f / (float)fps);
    float diagBugSpeed = sqrtf(0.5f * bugSpeed * bugSpeed);

    // -------------------------------------------------------
    // Platforms
    // -------------------------------------------------------
    std::vector<Platform> platforms = {
        {{  0.0f, -40.0f,  25.0f }, { 16.0f, 2.0f, 16.0f }, DARKGRAY},
        {{ 18.0f, -30.0f,   5.0f }, { 14.0f, 2.0f, 14.0f }, DARKGRAY},
        {{-12.0f, -20.0f, -10.0f }, { 12.0f, 2.0f, 12.0f }, DARKGRAY},
        {{ 10.0f, -10.0f, -28.0f }, { 16.0f, 2.0f, 16.0f }, DARKGRAY}
    };

    // -------------------------------------------------------
    // Lighting shader
    // -------------------------------------------------------
    Shader lightShader = LoadShader(
        "src/renderTesting/shaders/lighting.vs",
        "src/renderTesting/shaders/lighting.fs"
    );
    lightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightShader, "viewPos");

    int ambientLoc = GetShaderLocation(lightShader, "ambient");
    float ambient[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
    SetShaderValue(lightShader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

    Light sunLight = CreateLight(
        LIGHT_DIRECTIONAL,
        (Vector3){ 50, 50, 50 },
        (Vector3){ 0, 0, 0 },
        (Color){ 255, 240, 200, 255 },
        lightShader
    );
    Light fillLight = CreateLight(
        LIGHT_POINT,
        (Vector3){ -30, 20, -30 },
        (Vector3){ 0, 0, 0 },
        (Color){ 60, 70, 120, 255 },
        lightShader
    );

    // -------------------------------------------------------
    // ASCII post-processing shader
    // -------------------------------------------------------
    Shader asciiShader = LoadShader(
        0,
        TextFormat("src/renderTesting/shaders/ascii.fs", GLSL_VERSION)
    );

    int resolutionLoc = GetShaderLocation(asciiShader, "resolution");
    int fontSizeLoc   = GetShaderLocation(asciiShader, "fontSize");

    float fontSize      = 9.0f;
    float resolution[2] = { (float)screenWidth, (float)screenHeight };
    SetShaderValue(asciiShader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(asciiShader, fontSizeLoc, &fontSize, SHADER_UNIFORM_FLOAT);

    // -------------------------------------------------------
    // Materials — one per object color
    // -------------------------------------------------------
    Material bugMat = LoadMaterialDefault();
    bugMat.shader   = lightShader;
    bugMat.maps[MATERIAL_MAP_DIFFUSE].color = BROWN;

    Material floorMat = LoadMaterialDefault();
    floorMat.shader   = lightShader;
    floorMat.maps[MATERIAL_MAP_DIFFUSE].color = GRAY;

    Material platformMat = LoadMaterialDefault();
    platformMat.shader   = lightShader;
    platformMat.maps[MATERIAL_MAP_DIFFUSE].color = DARKGRAY;

    Material computerMat = LoadMaterialDefault();
    computerMat.shader   = lightShader;
    computerMat.maps[MATERIAL_MAP_DIFFUSE].color = BLACK;

    Material doorMat = LoadMaterialDefault();
    doorMat.shader   = lightShader;
    doorMat.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

    // -------------------------------------------------------
    // Meshes
    // -------------------------------------------------------
    Mesh sphereMesh   = GenMeshSphere(2.0f, 16, 16);
    Mesh planeMesh    = GenMeshPlane(100.0f, 100.0f, 1, 1);

    // Platforms — all same size so one mesh, drawn with different transforms
    // We generate one mesh per unique size since raylib meshes are fixed geometry
    std::vector<Mesh> platformMeshes;
    for (auto& p : platforms)
        platformMeshes.push_back(GenMeshCube(p.size.x, p.size.y, p.size.z));

    Mesh computerMesh = GenMeshCube(2.0f, 1.5f, 1.5f);
    Mesh doorMesh     = GenMeshCube(8.0f, 14.0f, 2.0f);

    // -------------------------------------------------------
    // Render texture
    // -------------------------------------------------------
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    SetTargetFPS(fps);

    // -------------------------------------------------------
    // Splash screen
    // -------------------------------------------------------
    const char* title =
        "$$$$$$$\\\\ $$$$$$$$\\\\$$$$$$$\\\\  $$\\\\  $$\\\\  $$$$$$\\\\\n$$  __$$\\\\$$  _____|$$  __$$\\\\ $$ |  $$ |$$  __ $$\\\\\n$$ |  $$ |$$ |      $$ |  $$  |$$ |  $$ |$$ /  \\\\__|\n$$ |  $$ |$$$$$\\\\   $$$$$$$\\\\ |$$ |  $$ |$$ |  $$$$\\\\\n$$ |  $$ |$$  __|   $$  __$$\\\\ $$ |  $$ |$$ |  \\\\_$$ |\n$$ |  $$ |$$ |      $$ |  $$  |$$ |  $$ |$$ |     $$ |\n$$$$$$$  |$$$$$$$$\\\\$$$$$$$   |\\\\$$$$$$  |\\\\$$$$$$   |\n\\\\_______/ \\\\________|\\\\______/   \\\\______/  \\\\______/";

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ENTER)) break;

        BeginDrawing();
            ClearBackground(BLACK);

            int lineHeight    = 18;
            int titleFontSize = 16;

            // Emulate a monospaced font by forcing a fixed width based on the '$' character
            int fixedCharWidth = MeasureText("/", titleFontSize);

            // Calculate max columns to perfectly center the entire block on the screen
            int maxCols = 0;
            int currentCols = 0;
            for (const char* p = title; *p != '\0'; p++) {
                if (*p == '\n') {
                    if (currentCols > maxCols) maxCols = currentCols;
                    currentCols = 0;
                } else {
                    currentCols++;
                }
            }
            if (currentCols > maxCols) maxCols = currentCols;

            int startX = GetScreenWidth() / 2 - (maxCols * fixedCharWidth) / 2;
            int y = GetScreenHeight() / 2 - 80;

            // Draw character by character on our rigid grid
            int row = 0;
            int col = 0;
            for (const char* p = title; *p != '\0'; p++) {
                if (*p == '\n') {
                    row++;
                    col = 0;
                } else {
                    // Only draw visible characters; we let the space just advance the column
                    if (*p != ' ') {
                        char cStr[2] = { *p, '\0' };
                        DrawText(cStr, startX + col * fixedCharWidth, y + row * lineHeight, titleFontSize, GREEN);
                    }
                    col++;
                }
            }

            if ((int)(GetTime() * 2) % 2 == 0) {
                DrawText("press enter to play",
                    GetScreenWidth() / 2 - MeasureText("press enter to play", 18) / 2,
                    GetScreenHeight() / 2 + 80,
                    18, DARKGREEN);
            }
        EndDrawing();
    }

    // -------------------------------------------------------
    // Main game loop
    // -------------------------------------------------------
    while (!WindowShouldClose())
    {
        UnloadRenderTexture(target);
        target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        float res[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };
        SetShaderValue(asciiShader, resolutionLoc, res, SHADER_UNIFORM_VEC2);

        // --- Update ---
        float currBugSpeed = bugSpeed
            - (IsKeyDown(KEY_LEFT_SHIFT)   * bugSpeed * 0.5f)
            + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);

        if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) &&
            (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) {
            currBugSpeed = diagBugSpeed
                - (IsKeyDown(KEY_LEFT_SHIFT)   * bugSpeed * 0.5f)
                + (IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed);
        }

        if (IsKeyDown(KEY_W)) bugPos.z -= currBugSpeed;
        if (IsKeyDown(KEY_A)) bugPos.x -= currBugSpeed;
        if (IsKeyDown(KEY_S)) bugPos.z += currBugSpeed;
        if (IsKeyDown(KEY_D)) bugPos.x += currBugSpeed;

        boundPos(bugPos,
            (float[2]){-50, 50},
            (float[2]){-100, 50},
            (float[2]){-50, 50});

        Vector3 currCamPos = {
            camPos.x + bugPos.x,
            camPos.y + bugPos.y,
            camPos.z + bugPos.z
        };
        boundPos(currCamPos,
            (float[2]){-45, 45},
            (float[2]){-45, 45},
            (float[2]){-45, 80});

        camera.position = currCamPos;
        camera.target   = bugPos;

        float camPosArr[3] = { currCamPos.x, currCamPos.y, currCamPos.z };
        SetShaderValue(lightShader,
            lightShader.locs[SHADER_LOC_VECTOR_VIEW],
            camPosArr, SHADER_UNIFORM_VEC3);

        UpdateLightValues(lightShader, sunLight);
        UpdateLightValues(lightShader, fillLight);

        if (IsWindowResized()) {
            UnloadRenderTexture(target);
            target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
            float res[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };
            SetShaderValue(asciiShader, resolutionLoc, res, SHADER_UNIFORM_VEC2);
        }

        if (IsKeyPressed(KEY_LEFT)  && fontSize > 9.0f)  fontSize -= 1.0f;
        if (IsKeyPressed(KEY_RIGHT) && fontSize < 15.0f) fontSize += 1.0f;
        SetShaderValue(asciiShader, fontSizeLoc, &fontSize, SHADER_UNIFORM_FLOAT);

        // --- Render 3D scene into render texture ---
        BeginTextureMode(target);
            ClearBackground(BLACK);
            BeginMode3D(camera);

                // Wireframe bounds
                DrawCubeWiresV({0,0,0}, {100, 100, 100}, DARKGRAY);

                // Floor
                DrawMesh(planeMesh, floorMat, MatrixTranslate(0, -50, 0));

                // OOB floor (no lighting needed, just flat)
                DrawPlane({0,-50,75}, {100, 50}, DARKGRAY);

                // Platforms
                for (int i = 0; i < (int)platforms.size(); i++) {
                    DrawMesh(platformMeshes[i], platformMat,
                        MatrixTranslate(
                            platforms[i].position.x,
                            platforms[i].position.y,
                            platforms[i].position.z));
                    DrawCubeWiresV(platforms[i].position, platforms[i].size, BLACK);
                }

                // Computer
                DrawMesh(computerMesh, computerMat,
                    MatrixTranslate(0.0f, -49.25f, 35.0f));
                DrawCubeWiresV({ 0.0f, -49.25f, 35.0f }, { 2.0f, 1.5f, 1.5f }, WHITE);

                // Door
                DrawMesh(doorMesh, doorMat,
                    MatrixTranslate(0.0f, -43.0f, -49.0f));
                DrawCubeWiresV({ 0.0f, -43.0f, -49.0f }, { 8.0f, 14.0f, 2.0f }, BLACK);

                // Bug
                DrawMesh(sphereMesh, bugMat,
                    MatrixTranslate(bugPos.x, bugPos.y, bugPos.z));

            EndMode3D();
        EndTextureMode();

        // --- Apply ASCII shader and draw to screen ---
        BeginDrawing();
            ClearBackground(BLACK);
            BeginShaderMode(asciiShader);
                DrawTextureRec(target.texture,
                    (Rectangle){ 0, 0,
                        (float)target.texture.width,
                        (float)-target.texture.height },
                    (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
            DrawFPS(10, 10);
        EndDrawing();
    }

    // Cleanup
    bugMat.shader      = { 0 };
    floorMat.shader    = { 0 };
    platformMat.shader = { 0 };
    computerMat.shader = { 0 };
    doorMat.shader     = { 0 };
    UnloadMaterial(bugMat);
    UnloadMaterial(floorMat);
    UnloadMaterial(platformMat);
    UnloadMaterial(computerMat);
    UnloadMaterial(doorMat);
    UnloadMesh(sphereMesh);
    UnloadMesh(planeMesh);
    UnloadMesh(computerMesh);
    UnloadMesh(doorMesh);
    for (auto& m : platformMeshes) UnloadMesh(m);
    UnloadShader(lightShader);
    UnloadShader(asciiShader);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}

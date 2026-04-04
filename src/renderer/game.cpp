#include <cmath>
//#include <iostream>
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
int main(void)
{
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
Vector3 bugPos = { 0.0f, -48.0f, 45.0f }; // Spawn point
Vector3 camPos = { 0.0f, 10.0f, 20.0f }; // Delta pos from bug
float bugSpeed = 0.15*(60/(float)fps); // Division is to have same speed at any fps
// For when two directions are being held at the same time for it to move the same speed
float diagBugSpeed = sqrt(0.5*bugSpeed*bugSpeed);

SetTargetFPS(fps); // Set our game to run at 60 frames-per-second
//--------------------------------------------------------------------------------------

// Main game loop
while (!WindowShouldClose()) // Detect window close button or ESC key
{
// Update
//----------------------------------------------------------------------------------

float currBugSpeed = bugSpeed - (
IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2
) + (
IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed
);

// Check if moving diagonal
if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) {
currBugSpeed = diagBugSpeed - (
IsKeyDown(KEY_LEFT_SHIFT) * bugSpeed * 1/2
) + (
IsKeyDown(KEY_LEFT_CONTROL) * bugSpeed
);
}

if (IsKeyDown(KEY_W)) { // Forward
bugPos.z -= currBugSpeed;
}
if (IsKeyDown(KEY_A)) { // Left
bugPos.x -= currBugSpeed;
}
if (IsKeyDown(KEY_S)) { // Backwards
bugPos.z += currBugSpeed;
}
if (IsKeyDown(KEY_D)) { // Right
bugPos.x += currBugSpeed;
}

// BOUNDS DONT ACCOUNT FOR BUG SIZE HE CLIPS THROUGH WALLS!!

// Bound the bug
boundPos(bugPos, (float[2]){-50,50}, (float[2]){-100,50}, (float[2]){-50,50});

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
DrawSphere(bugPos, 2, BROWN); //Vector3 centerPos, float radius, Color color

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

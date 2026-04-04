#include "raylib.h"
#include <string>
#include <vector>

int main() {
    InitWindow(1000, 700, "Terminal Prototype");
    SetTargetFPS(60);

    std::string input;
    std::vector<std::string> log = {
        "Welcome to the terminal.",
        "Type a command and press Enter."
    };

    while (!WindowShouldClose()) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126) {
                input += (char)key;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !input.empty()) {
            input.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (!input.empty()) {
                log.push_back("> " + input);

                if (input == "help") {
                    log.push_back("Commands: help, clear, hello");
                } else if (input == "hello") {
                    log.push_back("Hello, player.");
                } else if (input == "clear") {
                    log.clear();
                } else {
                    log.push_back("Unknown command.");
                }

                input.clear();
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangle(20, 20, 960, 660, BLACK);
        DrawRectangleLines(20, 20, 960, 660, GREEN);

        int y = 40;
        int start = (log.size() > 25) ? (int)log.size() - 25 : 0;
        for (int i = start; i < (int)log.size(); i++) {
            DrawText(log[i].c_str(), 40, y, 22, GREEN);
            y += 24;
        }

        DrawText(("> " + input).c_str(), 40, 640, 24, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
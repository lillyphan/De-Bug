#include "Typing.h"
#include "raylib.h"

Typing::Typing(PuzzleFile* file, Font font, ComputerTerminal& terminal)
    : file(file), terminal(terminal), finished(false)
{
    terminalFont = font;
    int blanks = countBlanks();

    if (file->typingSavedAnswers.size() == blanks) {
        userAnswers = file->typingSavedAnswers;
    } else {
        userAnswers = std::vector<std::string>(blanks, "");
    }
}

int Typing::countBlanks() const {
    int count = 0;
    for (const auto& line : file->starterCodeLines) {
        for (char c : line) {
            if (c == '_') count++;
        }
    }
    return count;
}

bool Typing::isMouseInside(int x, int y, int w, int h) const {
    Vector2 mouse = GetMousePosition();
    return mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h;
}

void Typing::update() {
    int activeBlank = 0;

    if (countBlanks() > 0) {
        activeBlank = 0;
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126 && !userAnswers.empty()) {
            userAnswers[activeBlank] += (char)key;
            terminal.playTypingSound();
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !userAnswers.empty() && !userAnswers[activeBlank].empty()) {
        userAnswers[activeBlank].pop_back();
        terminal.playTypingSound();
    }

    Rectangle saveButton = { 820, 620, 120, 40 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        isMouseInside((int)saveButton.x, (int)saveButton.y, (int)saveButton.width, (int)saveButton.height)) {
        file->typingSavedAnswers = userAnswers;
        file->saved = true;
        finished = true;
    }
}

void Typing::draw() {
    DrawTextEx(terminalFont, file->name.c_str(), (Vector2){40, 20}, 30, 1, GREEN);

    int startX = 40;
    int startY = 80;
    int lineHeight = 40;
    int fontSize = 28;
    int blankIndex = 0;

    for (int i = 0; i < (int)file->starterCodeLines.size(); i++) {
        int y = startY + i * lineHeight;
        const std::string& line = file->starterCodeLines[i];

        int cursorX = startX;
        std::string currentText;

        for (int j = 0; j < (int)line.size(); j++) {
            if (line[j] != '_') {
                currentText += line[j];
            } else {
                if (!currentText.empty()) {
                    DrawTextEx(terminalFont, currentText.c_str(), (Vector2){(float)cursorX, (float)y}, fontSize, 1, GREEN);
                    cursorX += (int)MeasureTextEx(terminalFont, currentText.c_str(), fontSize, 1).x;
                    currentText.clear();
                }

                int boxW = 200;
                int boxH = 30;
                DrawRectangleLines(cursorX, y - 4, boxW, boxH, GREEN);

                if (blankIndex < (int)userAnswers.size()) {
                    DrawTextEx(terminalFont, userAnswers[blankIndex].c_str(), (Vector2){(float)(cursorX + 6), (float)y}, fontSize, 1, GREEN);
                }

                cursorX += boxW + 8;
                blankIndex++;
            }
        }

        if (!currentText.empty()) {
            DrawTextEx(terminalFont, currentText.c_str(), (Vector2){(float)cursorX, (float)y}, fontSize, 1, GREEN);
        }
    }

    Rectangle saveButton = { 820, 620, 120, 40 };
    DrawRectangleLinesEx(saveButton, 2, GREEN);
    DrawTextEx(terminalFont, "Save", (Vector2){855, 628}, 20, 1, GREEN);
}

bool Typing::isFinished() const {
    return finished;
}
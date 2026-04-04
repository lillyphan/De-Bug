#include "Typing.h"
#include "raylib.h"

Typing::Typing(PuzzleFile* file)
    : file(file), finished(false)
{
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
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !userAnswers.empty() && !userAnswers[activeBlank].empty()) {
        userAnswers[activeBlank].pop_back();
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
    DrawText(file->name.c_str(), 40, 20, 30, GREEN);

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
                    DrawText(currentText.c_str(), cursorX, y, fontSize, GREEN);
                    cursorX += MeasureText(currentText.c_str(), fontSize);
                    currentText.clear();
                }

                int boxW = 200;
                int boxH = 30;
                DrawRectangleLines(cursorX, y - 4, boxW, boxH, GREEN);

                if (blankIndex < (int)userAnswers.size()) {
                    DrawText(userAnswers[blankIndex].c_str(), cursorX + 6, y, fontSize, GREEN);
                }

                cursorX += boxW + 8;
                blankIndex++;
            }
        }

        if (!currentText.empty()) {
            DrawText(currentText.c_str(), cursorX, y, fontSize, GREEN);
        }
    }

    Rectangle saveButton = { 820, 620, 120, 40 };
    DrawRectangleLinesEx(saveButton, 2, GREEN);
    DrawText("Save", 855, 628, 20, GREEN);
}

bool Typing::isFinished() const {
    return finished;
}
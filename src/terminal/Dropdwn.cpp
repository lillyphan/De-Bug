#include "Dropdwn.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../../raygui/raygui.h"

Dropdwn::Dropdwn(PuzzleFile* file, Font font)
    : file(file), finished(false)
{
    terminalFont = font;
    int blanks = countBlanks();

    if ((int)file->dropdownSavedIndices.size() == blanks) {
        selectedIndices = file->dropdownSavedIndices;
    } else {
        selectedIndices = std::vector<int>(blanks, 0);
    }

    editModes = std::vector<char>(blanks, 0);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x000000ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x00ff00ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x222222ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x00ff00ff);

    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x333333ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0x00ff00ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x00ff00ff);

    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x444444ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0x00ff00ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x00ff00ff);
}

int Dropdwn::countBlanks() const {
    int count = 0;

    for (const auto& line : file->starterCodeLines) {
        for (char c : line) {
            if (c == '_') {
                count++;
            }
        }
    }

    return count;
}

std::string Dropdwn::joinOptions(const std::vector<std::string>& options) const {
    std::string joined;

    for (int i = 0; i < (int)options.size(); i++) {
        joined += options[i];
        if (i + 1 < (int)options.size()) {
            joined += ";";
        }
    }

    return joined;
}

bool Dropdwn::isMouseInside(int x, int y, int w, int h) const {
    Vector2 mouse = GetMousePosition();
    return mouse.x >= x && mouse.x <= x + w &&
           mouse.y >= y && mouse.y <= y + h;
}

void Dropdwn::update() {
    Rectangle saveButton = { 820, 620, 120, 40 };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        isMouseInside((int)saveButton.x, (int)saveButton.y,
                      (int)saveButton.width, (int)saveButton.height)) {
        file->dropdownSavedIndices = selectedIndices;
        file->saved = true;
        finished = true;
    }
}

void Dropdwn::draw() {
    DrawTextEx(terminalFont, file->name.c_str(), (Vector2){40, 20}, 30, 1, GREEN);

    int startX = 40;
    int startY = 80;
    int lineHeight = 50;
    int fontSize = 28;
    int blankIndex = 0;

    for (int lineIndex = 0; lineIndex < (int)file->starterCodeLines.size(); lineIndex++) {
        int y = startY + lineIndex * lineHeight;
        const std::string& line = file->starterCodeLines[lineIndex];

        int cursorX = startX;
        std::string currentText = "";

        for (int charIndex = 0; charIndex < (int)line.size(); charIndex++) {
            if (line[charIndex] != '_') {
                currentText += line[charIndex];
            } else {
                if (!currentText.empty()) {
                    DrawTextEx(terminalFont, currentText.c_str(), (Vector2){(float)cursorX, (float)y}, fontSize, 1, GREEN);
                    cursorX += MeasureText(currentText.c_str(), fontSize);
                    currentText.clear();
                }

                if (blankIndex < (int)file->dropdownOptions.size()) {
                    std::string optionsText = joinOptions(file->dropdownOptions[blankIndex]);

                    Rectangle rect = {
                        (float)cursorX,
                        (float)(y - 4),
                        180,
                        34
                    };

                    bool editMode = (editModes[blankIndex] != 0);

                    if (GuiDropdownBox(rect,
                                       optionsText.c_str(),
                                       &selectedIndices[blankIndex],
                                       editMode)) {
                        editModes[blankIndex] = !editMode;
                    }

                    cursorX += 190;
                }

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

bool Dropdwn::isFinished() const {
    return finished;
}
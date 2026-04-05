#ifndef TYPING_H
#define TYPING_H

#include "Terminal.h"
#include "PuzzleFile.h"
#include <string>
#include <vector>
#include "raylib.h"
#include "core/headers/ComputerTerminal.h"

class Typing : public Terminal {
private:
    PuzzleFile* file;
    std::vector<std::string> userAnswers;
    bool finished;

    int countBlanks() const;
    bool isMouseInside(int x, int y, int w, int h) const;

public:
    explicit Typing(PuzzleFile* file, Font font, ComputerTerminal& terminal);
    ComputerTerminal& terminal;

    void update() override;
    void draw() override;
    bool isFinished() const override;
};

#endif
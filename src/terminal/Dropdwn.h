#ifndef DROPDWN_H
#define DROPDWN_H

#include "Terminal.h"
#include "PuzzleFile.h"
#include <string>
#include <vector>

class Dropdwn : public Terminal {
private:
    PuzzleFile* file;

    // One selected option per blank
    std::vector<int> selectedIndices;

    // 0 = closed, 1 = open
    std::vector<char> editModes;

    bool finished;

    int countBlanks() const;
    std::string joinOptions(const std::vector<std::string>& options) const;
    bool isMouseInside(int x, int y, int w, int h) const;

public:
    explicit Dropdwn(PuzzleFile* file);

    void update() override;
    void draw() override;
    bool isFinished() const override;
};

#endif
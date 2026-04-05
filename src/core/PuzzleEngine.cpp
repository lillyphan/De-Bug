#include "headers/PuzzleEngine.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

// -------------------------------------------------------
// Public
// -------------------------------------------------------
bool PuzzleEngine::validate(const PuzzleState& ps, PuzzleType type)
{
    switch (type) {
        case PuzzleType::Typing:     return validateCode(ps);
        case PuzzleType::Dropdown: return validateDropdown(ps);
    }
    return false;
}

// -------------------------------------------------------
// Code puzzle — normalise both strings and compare
// -------------------------------------------------------
bool PuzzleEngine::validateCode(const PuzzleState& ps)
{
    return normalise(ps.playerCode) == normalise(ps.correctCode);
}

// -------------------------------------------------------
// Dropdown puzzle — check selected sequence matches correctSequence
// -------------------------------------------------------
bool PuzzleEngine::validateDropdown(const PuzzleState& ps)
{
    //must have the same number of dropdowns as expected answers
    if (ps.dropdowns.size() != ps.correctSequence.size())
        return false;

    for (int i = 0; i < (int)ps.dropdowns.size(); i++) {
        const Dropdown& d = ps.dropdowns[i];

        //nothing selected in this slot
        if (d.selectedIndex < 0 ||
            d.selectedIndex >= (int)d.options.size())
            return false;

        //compare selected option against correct answer at this position
        if (normalise(d.selected()) != normalise(ps.correctSequence[i]))
            return false;
    }
    return true;
}

// -------------------------------------------------------
// Normalise — lowercase, collapse all whitespace to single spaces, trim
// -------------------------------------------------------
std::string PuzzleEngine::normalise(const std::string& s)
{
    std::string out;
    bool lastWasSpace = true; //true to trim leading whitespace

    for (unsigned char c : s) {
        if (std::isspace(c)) {
            if (!lastWasSpace)
                out += ' ';
            lastWasSpace = true;
        } else {
            out += std::tolower(c);
            lastWasSpace = false;
        }
    }

    //trim trailing space
    if (!out.empty() && out.back() == ' ')
        out.pop_back();

    return out;
}
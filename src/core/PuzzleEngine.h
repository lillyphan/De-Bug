#pragma once
#include "GameState.h"
#include "PuzzleType.h"

// -------------------------------------------------------
// PuzzleEngine — validates puzzle submissions
// -------------------------------------------------------
class PuzzleEngine {
public:
    explicit PuzzleEngine(GameState& state) : m_state(state) {}

    //will return true if the player's current submission is correct
    //commandParser calls this after writing playerCode / selectedIndex into PuzzleState
    bool validate(const PuzzleState& ps, PuzzleType type);

private:
    GameState& m_state;

    //type-specific validators
    bool validateCode    (const PuzzleState& ps);
    bool validateDropdown(const PuzzleState& ps);

    //string normalisation, strips whitespace, lowercases
    std::string normalise(const std::string& s);
};
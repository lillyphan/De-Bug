#pragma once
#include <string>
#include <vector>
#include "GameState.h"

// -------------------------------------------------------
// CommandResult — everything the terminal needs to display
// -------------------------------------------------------
struct CommandResult {
    std::vector<std::string> lines;   // pushed into terminal log
    bool                     solved = false;
    bool                     failed = false;  // ran out of attempts
    bool                     clear  = false;  // player typed "clear"
};

// -------------------------------------------------------
// CommandParser — routes terminal input to PuzzleEngine
// -------------------------------------------------------
class CommandParser {
public:
    explicit CommandParser(GameState& state) : m_state(state) {}

    // Call this whenever the player hits Enter in the terminal.
    // Pass the raw input string and the current dropdown selections.
    CommandResult submit(const std::string&              rawInput,
                         const std::vector<std::string>& dropdownSelections = {});

private:
    GameState& m_state;

    // Built-in terminal commands (always available)
    CommandResult handleHelp();
    CommandResult handleClear();
    CommandResult handleHint();

    // Puzzle submission — called when player types "run" or "submit"
    CommandResult handleRun(const std::string&              code,
                            const std::vector<std::string>& dropdownSelections);

    // Helpers
    CommandResult unknownCommand(const std::string& input);
    std::string   trim(const std::string& s);
    std::string   toLower(const std::string& s);
};
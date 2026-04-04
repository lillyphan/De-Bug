#pragma once
#include <string>
#include <vector>
#include "GameState.h"

// -------------------------------------------------------
// CommandResult — just everything the terminal needs to display
// -------------------------------------------------------
struct CommandResult {
    std::vector<std::string> lines;
    bool                     solved = false;
    bool                     clear  = false;
};

// -------------------------------------------------------
// CommandParser — routes terminal input to PuzzleEngine
// -------------------------------------------------------
class CommandParser {
public:
    explicit CommandParser(GameState& state) : m_state(state) {}

    //call this whenever the player hits enter in the terminal
    CommandResult submit(const std::string&              rawInput,
                         const std::vector<std::string>& dropdownSelections = {},
                         const std::string&              editorCode = "");

    Room* currentRoom() { return m_state.currentRoom(); }

private:
    GameState& m_state;

    CommandResult handleHelp();
    CommandResult handleClear();
    CommandResult handleHint();
    CommandResult handleRun(const std::string&              playerCode,
                            const std::vector<std::string>& dropdownSelections);

    CommandResult unknownCommand(const std::string& input);
    std::string   trim(const std::string& s);
    std::string   toLower(const std::string& s);
};
#include "headers/CommandParser.h"
#include "headers/PuzzleEngine.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

// -------------------------------------------------------
// Public entry point
// -------------------------------------------------------
CommandResult CommandParser::submit(const std::string &rawInput, const std::vector<std::string> &dropdownSelections, const std::string &editorCode)
{
    std::string input = trim(rawInput);
    if (input.empty()) return {};

    std::string cmd = toLower(input);

    //built-in commands (that are always available regardless of puzzle state)
    if (cmd == "help") return handleHelp();
    if (cmd == "clear") return handleClear();
    if (cmd == "hint") return handleHint();
    if (cmd == "exit") return handleExit();

    //puzzle submission
    if (cmd == "run")
    return handleRun(editorCode, dropdownSelections);

    return unknownCommand(input);
}

// -------------------------------------------------------
// Built-in handlers
// -------------------------------------------------------
CommandResult CommandParser::handleHelp()
{
    Room* room = m_state.currentRoom();
    CommandResult result;

    result.lines.push_back("--- help ---");
    //print all available
    std::vector<std::string> commands = room->puzzle.availableCommands;
    for (int i = 0; i < commands.size(); i++){
        std::string description = commands[i] + " - " + m_state.commandDescriptions[commands[i]];
        result.lines.push_back(description);
    }
    return result;
}

CommandResult CommandParser::handleClear()
{
    CommandResult result;
    result.clear = true;
    return result;
}

CommandResult CommandParser::handleHint()
{
    CommandResult result;
    Room* room = m_state.currentRoom();

    if (!room) {
        result.lines.push_back("No active room.");
        return result;
    }

    PuzzleState& ps = room->puzzle;

    if (ps.status == PuzzleStatus::Solved) {
        result.lines.push_back("You already solved this one.");
        return result;
    }

    //hint lives in RoomDescriptor, its gunna be passed through to PuzzleState
    if (!ps.hint.empty()) {
        result.lines.push_back("Hint: " + ps.hint);
    } else {
        result.lines.push_back("No hint available for this puzzle.");
    }

    return result;
}

CommandResult CommandParser::handleExit()
{
    CommandResult result;
    result.exit = true;
    return result;
}

// -------------------------------------------------------
// Puzzle submission
// -------------------------------------------------------
CommandResult CommandParser::handleRun(const std::string &playerCode, const std::vector<std::string> &dropdownSelections)
{
    CommandResult result;
    Room* room = m_state.currentRoom();

    //guard against if there's no active room, report -- hopefully this should never happen
    if (!room) {
        result.lines.push_back("Error: no active room.");
        return result;
    }

    PuzzleState& ps = room->puzzle;

    //check if puzzle is even active or solved
    if (ps.status == PuzzleStatus::Locked) {
        result.lines.push_back("This terminal is locked.");
        return result;
    }
    if (ps.status == PuzzleStatus::Solved) {
        result.lines.push_back("Puzzle already solved. Find the door.");
        return result;
    }

    //make sure all of the dropdowns are selected first
    if (!ps.allDropdownsSelected()) {
        result.lines.push_back("Select an option from every dropdown first.");
        return result;
    }

    //passed all checks, write player input into PuzzleState
    ps.playerCode = playerCode;
    //loops thorugh all the dropdown slections  
    for (int i = 0; i < (int)dropdownSelections.size() &&
                    i < (int)ps.dropdowns.size(); i++) {
        //find and set the matching index for each selection
        const Dropdown& d = ps.dropdowns[i];
        for (int j = 0; j < (int)d.options.size(); j++) {
            if (d.options[j] == dropdownSelections[i]) {
                //assign the selected dropdowns and pass it back
                ps.dropdowns[i].selectedIndex = j;
                break;
            }
        }
    }
    
    //pass to the puzzleEngine to validate the result
    PuzzleEngine engine(m_state); //puzzle engine according to the room (state)
    bool correct = engine.validate(ps, ps.type);

    if (correct) {
        m_state.markPuzzleSolved();
        // result.lines.push_back("Correct. The door is now unlocked.");
        result.solved = true;
    } else {
        m_state.recordAttempt(playerCode);
    }

    return result;
}

// -------------------------------------------------------
// Helpers
// -------------------------------------------------------

//if the player gives an unknown command 
CommandResult CommandParser::unknownCommand(const std::string& input)
{
    CommandResult result;
    result.lines.push_back("Unknown command: " + input);
    result.lines.push_back("Type 'help' to see available commands.");
    return result;
}

//get rid of whitespace
std::string CommandParser::trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

//lowercase helper
std::string CommandParser::toLower(const std::string& s)
{
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}



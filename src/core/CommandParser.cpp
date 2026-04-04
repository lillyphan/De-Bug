#include "CommandParser.h"
#include "PuzzleEngine.h"
#include <algorithm>
#include <cctype>
#include <sstream>

// -------------------------------------------------------
// Public entry point
// -------------------------------------------------------
CommandResult CommandParser::submit(const std::string&rawInput, const std::vector<std::string>& dropdownSelections)
{
    std::string input = trim(rawInput);
    if (input.empty()) return {};

    std::string cmd = toLower(input);

    //built-in commands (that are always available regardless of puzzle state)
    if (cmd == "help") return handleHelp();
    if (cmd == "clear") return handleClear();
    if (cmd == "hint") return handleHint();

    //puzzle submission
    if (cmd == "run")
        return handleRun(input, dropdownSelections);

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
    result.lines.push_back("  run            run your code and selected commands");
    result.lines.push_back("  hint           show a hint for the current puzzle");
    result.lines.push_back("  clear          clear the terminal");

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

// -------------------------------------------------------
// Puzzle submission
// -------------------------------------------------------
CommandResult CommandParser::handleRun(const std::string& code, const std::vector<std::string>& dropdownSelections)
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
    ps.playerCode = ps.startingCode;  //startingCode is what they edited from
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
    PuzzleEngine engine(m_state);     //puzzle engine according to the room (state)
    bool correct = engine.validate(ps);

    if (correct) {
        m_state.markPuzzleSolved();
        // result.lines.push_back("Correct. The door is now unlocked.");
        result.solved = true;
    } else {
        m_state.recordAttempt(code);
    }

    return result;
}

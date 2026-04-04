#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GameState.h"
#include "PuzzleType.h"

// -------------------------------------------------------
// PuzzleLoader
// Reads room .txt config files, constructs Room objects,
// and adds them directly to GameState.
//
// Expected file format (one field per line, in order):
//   Line 0:  prompt text
//   Line 1:  filenames, space-separated (e.g. "crash.py helper.py")
//   Line 2:  hint text
//   Line 3:  available commands, space-separated (e.g. "ls nano")
//   Line 4:  puzzle type ("dp" = dropdown, "tp" = typing)
//   Line 5:  number of prewritten code lines (integer)
//   Lines 6..6+n-1: prewritten code lines (the startingCode block)
//   Next line (dp only): dropdown options, space-separated
//   Last line: solution
//     - dp: space-separated correct sequence (e.g. "!crash")
//     - tp: the exact correct code string
// -------------------------------------------------------
class PuzzleLoader {
public:

    // Loads all room configs from a directory and the shared commands file
    // into gameState. Rooms are keyed by id (e.g. "room_01").
    // Stops loading when the next expected room file is not found.
    static void loadAll(GameState&         gameState,
                        const std::string& roomsDir,
                        const std::string& commandsFilePath)
    {
        gameState.commandDescriptions = loadCommandsFile(commandsFilePath);

        int index = 1;
        while (true) {
            std::string id       = "room_" + zeroPad(index, 2);
            std::string filePath = roomsDir + "/" + id + ".txt";

            Room room;
            room.id = id;
            if (!parseRoomFile(filePath, room)) break;

            gameState.rooms[id] = room;
            index++;
        }

        if (gameState.rooms.empty())
            std::cerr << "[PuzzleLoader] Warning: no rooms loaded from: "
                      << roomsDir << "\n";
    }

    // Loads a single room file and adds it to gameState.
    // Useful for hot-reloading a specific room during development.
    static bool loadRoom(GameState&         gameState,
                         const std::string& filePath,
                         const std::string& roomId)
    {
        Room room;
        room.id = roomId;
        if (!parseRoomFile(filePath, room)) return false;
        gameState.rooms[roomId] = room;
        return true;
    }

private:

    static bool parseRoomFile(const std::string& filePath, Room& room)
    {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "[PuzzleLoader] Could not open: " << filePath << "\n";
            return false;
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line))
            lines.push_back(line);
        file.close();

        if (lines.size() < 6) {
            std::cerr << "[PuzzleLoader] File too short: " << filePath << "\n";
            return false;
        }

        PuzzleState& ps = room.puzzle;
        int cursor = 0;

        // Line 0 — prompt
        ps.prompt = lines[cursor++];

        // Line 1 — filenames, space-separated
        ps.filenames = splitBySpace(lines[cursor++]);

        // Line 2 — hint
        ps.hint = lines[cursor++];

        // Line 3 — available commands, space-separated
        ps.availableCommands = splitBySpace(lines[cursor++]);

        // Line 4 — puzzle type
        std::string typeStr = lines[cursor++];
        if      (typeStr == "dp") ps.type = PuzzleType::Dropdown;
        else if (typeStr == "tp") ps.type = PuzzleType::Typing;
        else {
            std::cerr << "[PuzzleLoader] Unknown puzzle type '"
                      << typeStr << "' in: " << filePath << "\n";
            return false;
        }

        // Line 5 — number of prewritten code lines
        int codeLineCount = 0;
        try {
            codeLineCount = std::stoi(lines[cursor++]);
        } catch (...) {
            std::cerr << "[PuzzleLoader] Invalid code line count in: "
                      << filePath << "\n";
            return false;
        }

        if ((int)lines.size() < cursor + codeLineCount + 1) {
            std::cerr << "[PuzzleLoader] Not enough lines for code block in: "
                      << filePath << "\n";
            return false;
        }

        // Lines 6..6+n-1 — prewritten code block
        std::string codeBlock;
        for (int i = 0; i < codeLineCount; i++) {
            if (i > 0) codeBlock += "\n";
            codeBlock += lines[cursor++];
        }
        ps.startingCode = codeBlock;
        ps.playerCode   = codeBlock;  // player starts with the prewritten code

        // --- Dropdown-specific ---
        if (ps.type == PuzzleType::Dropdown) {
            if (cursor >= (int)lines.size()) {
                std::cerr << "[PuzzleLoader] Missing dropdown options in: "
                          << filePath << "\n";
                return false;
            }

            Dropdown dropdown;
            dropdown.label   = "Select command";
            dropdown.options = splitBySpace(lines[cursor++]);
            ps.dropdowns.push_back(dropdown);

            if (cursor >= (int)lines.size()) {
                std::cerr << "[PuzzleLoader] Missing solution in: "
                          << filePath << "\n";
                return false;
            }

            // Correct sequence — space-separated, order matters
            ps.correctSequence = splitBySpace(lines[cursor++]);
        }

        // --- Typing-specific ---
        else if (ps.type == PuzzleType::Typing) {
            if (cursor >= (int)lines.size()) {
                std::cerr << "[PuzzleLoader] Missing solution in: "
                          << filePath << "\n";
                return false;
            }
            ps.correctCode = lines[cursor++];
        }

        ps.status = PuzzleStatus::Locked;
        return true;
    }

static std::unordered_map<std::string, std::string>
    loadCommandsFile(const std::string& filePath)
    {
        std::unordered_map<std::string, std::string> commands;
        std::ifstream file(filePath);
 
        if (!file.is_open()) {
            std::cerr << "[PuzzleLoader] Could not open commands file: "
                      << filePath << "\n";
            return commands;
        }
 
        // Format: command name on one line, description on the next
        std::string line;
        while (std::getline(file, line)) {
            std::string name = trim(line);
            if (name.empty()) continue;
 
            std::string desc;
            if (std::getline(file, desc))
                desc = trim(desc);
 
            commands[name] = desc;
        }
 
        return commands;
    }

    // zeroPad(1, 2) -> "01", zeroPad(12, 2) -> "12"
    static std::string zeroPad(int value, int width)
    {
        std::string s = std::to_string(value);
        while ((int)s.size() < width) s = "0" + s;
        return s;
    }

    static std::vector<std::string> splitBySpace(const std::string& s)
    {
        std::vector<std::string> tokens;
        std::istringstream stream(s);
        std::string token;
        while (stream >> token)
            tokens.push_back(token);
        return tokens;
    }

    static std::string trim(const std::string& s)
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end   = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }
};
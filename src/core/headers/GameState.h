#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "PuzzleType.h"

// -------------------------------------------------------
// PuzzleState — tracks one puzzle's progress
// -------------------------------------------------------
enum class PuzzleStatus {
    Locked, // not yet accessible
    Active, // player is working on it
    Solved, // completed successfully
};

//dropdown widget, its options and what the player picked
struct Dropdown {
    std::string label;
    std::vector<std::string> options;  // populated from RoomDescriptor
    int selectedIndex = -1;  // -1 = nothing chosen yet

    std::string selected() const {
        if (selectedIndex < 0 || selectedIndex >= (int)options.size())
            return "";
        return options[selectedIndex];
    }
};

struct PuzzleState {
    PuzzleStatus status = PuzzleStatus::Locked; // start as locked
    PuzzleType type = PuzzleType::Typing;

    std::vector<Dropdown> dropdowns;    // one per dropdown widget, in order
    std::vector<std::string> correctSequence; //correct sequence of dropdown buttons

    std::string  playerCode; //edited code
    std::string  startingCode; //starting
    std::string  correctCode; //the "key"

    std::string hint;
    std::string prompt;
    std::vector<std::string> availableCommands;
    std::vector<std::string> filenames;

    //in here, add two terminals as instances, initialize them using prexisting info in the puzzle state

    // convenience — did the player pick something in every dropdown?
    bool allDropdownsSelected() const {
        for (auto& d : dropdowns)
            if (d.selectedIndex < 0) return false;
        return true;
    }
};

// -------------------------------------------------------
// Room — one puzzle room
// -------------------------------------------------------
struct Room {
    std::string id;
    std::string name;
    bool doorUnlocked = false;

    PuzzleState puzzle; // one puzzle per room

    //check is the puzzle is solved
    bool isPuzzleSolved() const {
        return puzzle.status == PuzzleStatus::Solved;
    }
};

// -------------------------------------------------------
// GameState — idk important or something
// -------------------------------------------------------
struct GameState {
    std::unordered_map<std::string, Room>    rooms;
    std::unordered_map<std::string, std::string>    commandDescriptions;
    std::string                              activeRoomId;

    // ****************** Room access helpers ******************

    Room* currentRoom() {
        auto it = rooms.find(activeRoomId);
        return (it != rooms.end()) ? &it->second : nullptr;
    }

    Room* getRoom(const std::string& id) {
        auto it = rooms.find(id);
        return (it != rooms.end()) ? &it->second : nullptr;
    }

    // ****************** Puzzle helpers ******************

    //call this when the player solves the current room's puzzle
    void markPuzzleSolved() {
        Room* room = currentRoom();
        if (!room) return;
        room->puzzle.status = PuzzleStatus::Solved;
        room->doorUnlocked  = true;
    }

    //call this on a failed submission
    void recordAttempt(const std::string& submittedCode) {
        Room* room = currentRoom();
        if (!room) return;

        PuzzleState& ps = room->puzzle;
        
        if (!submittedCode.empty())
            room->puzzle.playerCode = submittedCode;
    }
};
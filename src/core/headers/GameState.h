#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "PuzzleType.h"

// -------------------------------------------------------
// RoomObject — a thing in the room that can change state
// -------------------------------------------------------
struct RoomObject {
    std::string id;  // e.g. "door_01", "platform_a"
    std::string type;  // "door", "platform", "wall", "switch"
    bool active; // visible / enabled
    bool passable; // can the player walk through it?
    float posX, posY, posZ;
};

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
    std::string id;
    PuzzleStatus status = PuzzleStatus::Locked; // start as locked
    PuzzleType type = PuzzleType::Typing;
    int attempts = 0;

    std::vector<Dropdown> dropdowns;    // one per dropdown widget, in order
    std::vector<std::string> correctSequence; //correct sequence of dropdown buttons

    std::string  playerCode; //edited code
    std::string  startingCode; //starting
    std::string  correctCode; //the "key"

    std::string hint;
    std::string prompt;
    std::vector<std::string> availableCommands;
    std::vector<std::string> filenames;

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

    std::vector<RoomObject> objects;
    PuzzleState puzzle; // one puzzle per room

    //convenience helpers
    RoomObject* findObject(const std::string& objId) {
        for (auto& obj : objects)
            if (obj.id == objId) return &obj;
        return nullptr;
    }

    //check is the puzzle is solved
    bool isPuzzleSolved() const {
        return puzzle.status == PuzzleStatus::Solved;
    }
};

// -------------------------------------------------------
// PlayerState — where the player is and what they've done
// -------------------------------------------------------
struct PlayerState {
    //maybe get this from raylib
    float posX = 0.f, posY = 0.f, posZ = 0.f;

    std::string currentRoomId;
    bool terminalOpen = false;  //is the CLI overlay active?
};

// -------------------------------------------------------
// GameState — idk important or something
// -------------------------------------------------------
struct GameState {
    PlayerState                              player;
    std::unordered_map<std::string, Room>    rooms;
    std::unordered_map<std::string, std::string>    commandDescriptions;
    std::string                              activeRoomId;
    int                                      currentLevel = 0;

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
        ps.playerCode = submittedCode;
    }

    // ****************** Navigation ******************

    //returns false if the door isn't unlocked yet
    bool tryAdvanceRoom(const std::string& nextRoomId) {
        Room* room = currentRoom();
        if (!room || !room->doorUnlocked) return false;

        activeRoomId          = nextRoomId;
        player.currentRoomId  = nextRoomId;
        return true;
    }

    //object mutation (called by EventBus listeners)
    void setObjectPassable(const std::string& objId, bool passable) {
        Room* room = currentRoom();
        if (!room) return;
        RoomObject* obj = room->findObject(objId);
        if (obj) obj->passable = passable;
    }

    void setObjectActive(const std::string& objId, bool active) {
        Room* room = currentRoom();
        if (!room) return;
        RoomObject* obj = room->findObject(objId);
        if (obj) obj->active = active;
    }
};
#include <iostream>
#include "headers/GameState.h"
#include "headers/PuzzleType.h"
#include "headers/PuzzleLoader.h"

void printPuzzleState(const PuzzleState& ps) {
    std::cout << "  prompt:    " << ps.prompt << "\n";
    std::cout << "  hint:      " << ps.hint << "\n";
    std::cout << "  type:      " << (ps.type == PuzzleType::Dropdown ? "dp" : "tp") << "\n";

    std::cout << "  filenames: ";
    for (auto& f : ps.filenames) std::cout << f << " ";
    std::cout << "\n";

    std::cout << "  commands:  ";
    for (auto& c : ps.availableCommands) std::cout << c << " ";
    std::cout << "\n";

    std::cout << "  startingCode:\n" << ps.startingCode << "\n";

    if (ps.type == PuzzleType::Dropdown) {
        std::cout << "  dropdown options: ";
        for (auto& d : ps.dropdowns)
            for (auto& o : d.options) std::cout << o << " ";
        std::cout << "\n";
        std::cout << "  correctSequence: ";
        for (auto& s : ps.correctSequence) std::cout << s << " ";
        std::cout << "\n";
    } else {
        std::cout << "  correctCode: " << ps.correctCode << "\n";
    }
}
int main() {
    GameState gameState;
    PuzzleLoader::loadAll(gameState, "src/assets/rooms", "src/assets/commands.txt");

    // --- Print commands ---
    std::cout << "=== Commands ===\n";
    for (auto& [name, desc] : gameState.commandDescriptions)
        std::cout << "  " << name << ": " << desc << "\n";
    std::cout << "\n";

    // --- Print each room ---
    std::cout << "=== Rooms (" << gameState.rooms.size() << " loaded) ===\n";
    for (auto& [id, room] : gameState.rooms) {
        std::cout << "\n[" << id << "]\n";
        printPuzzleState(room.puzzle);
    }

    return 0;
}
#include "raylib.h"
#include "headers/GameState.h"
#include "headers/PuzzleType.h"
#include "headers/PuzzleLoader.h"
#include "terminal/PuzzleFile.h"
#include "headers/CommandParser.h"
#include "terminal/CommandPrompt.h"
#include "terminal/Dropdwn.h"
#include "terminal/Typing.h"
#include <string>
#include <vector>
#include <memory>

int main()
{
    InitWindow(1000, 700, "Puzzle Room");
    SetTargetFPS(60);

    // --- Load everything into GameState ---
    Font terminalFont = LoadFont("src/assets/fonts/terminal.ttf");
    GameState gameState;
    PuzzleLoader::loadAll(gameState, "src/assets/rooms", "src/assets/commands.txt");
    gameState.activeRoomId        = "room_02";
    gameState.player.currentRoomId = "room_02";

    // --- Build PuzzleFiles from the active room's PuzzleState ---
    // These are what Dropdwn and Typing read from
    std::vector<PuzzleFile> puzzleFiles;
    Room* room = gameState.currentRoom();
    if (room) {
        for (auto& filename : room->puzzle.filenames)
            puzzleFiles.push_back(PuzzleFile::fromPuzzleState(room->puzzle, filename));
    }

    // --- Set puzzle to active so CommandParser won't reject submissions ---
    if (room) room->puzzle.status = PuzzleStatus::Active;

    // --- Set up parser and terminal ---
    CommandParser  parser(gameState);
    CommandPrompt  commandPrompt(parser, &puzzleFiles, terminalFont);

    // Active editor — null when CommandPrompt is showing
    std::unique_ptr<Terminal> activeEditor = nullptr;

    while (!WindowShouldClose()) {

        // --- Update ---
        if (activeEditor) {
            activeEditor->update();

            if (activeEditor->isFinished()) {
                // Editor saved — sync answers back to PuzzleState
                // so CommandParser/PuzzleEngine can validate on "run"
                Room* r = gameState.currentRoom();
                if (r) {
                    for (auto& pf : puzzleFiles)
                        pf.syncToPuzzleState(r->puzzle);
                }
                activeEditor = nullptr;
                // Return to CommandPrompt
            }
        } else {
            commandPrompt.update();

            // CommandPrompt wants to open a file editor
            if (commandPrompt.wantsToOpenFile()) {
                std::string fname = commandPrompt.getRequestedOpenFile();
                commandPrompt.clearOpenRequest();

                // Find the matching PuzzleFile and open the right editor
                for (auto& pf : puzzleFiles) {
                    if (pf.name == fname) {
                        if (pf.type == FileType::Dropdwn)
                            activeEditor = std::make_unique<Dropdwn>(&pf, terminalFont);
                        else
                            activeEditor = std::make_unique<Typing>(&pf, terminalFont);
                        break;
                    }
                }
            }
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (activeEditor)
            activeEditor->draw();
        else
            commandPrompt.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
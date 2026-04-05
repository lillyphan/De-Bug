#include "headers/ComputerTerminal.h"

#include "headers/PuzzleLoader.h"
#include "headers/PuzzleType.h"
#include "terminal/Dropdwn.h"
#include "terminal/Typing.h"

#include "raylib.h"

ComputerTerminal::ComputerTerminal()
    : openFlag(false),
    terminalFont(LoadFont("src/assets/fonts/terminal.ttf")) {
    PuzzleLoader::loadAll(gameState, "src/assets/puzzles", "src/assets/commands.txt");
    computerSound = LoadMusicStream("src/assets/sounds/computer.mp3");
    typingSounds[0] = LoadSound("src/assets/sounds/type1.mp3");
    typingSounds[1] = LoadSound("src/assets/sounds/type2.mp3");
    typingSounds[2] = LoadSound("src/assets/sounds/type3.mp3");
    typingSounds[3] = LoadSound("src/assets/sounds/type4.mp3");
}

void ComputerTerminal::rebuildPuzzleFiles() {
    puzzleFiles.clear();

    Room* room = gameState.currentRoom();
    if (!room) return;

    for (const auto& filename : room->puzzle.filenames) {
        puzzleFiles.push_back(PuzzleFile::fromPuzzleState(room->puzzle, filename));
    }
}

void ComputerTerminal::open(const std::string& roomId) {
    PlayMusicStream(computerSound);
    gameState.activeRoomId = roomId;
    // gameState.player.currentRoomId = roomId;

    Room* room = gameState.currentRoom();
    if (room) {
        room->puzzle.status = PuzzleStatus::Active;
    }

    rebuildPuzzleFiles();

    parser = std::make_unique<CommandParser>(gameState);
    commandPrompt = std::make_unique<CommandPrompt>(*parser, &puzzleFiles, terminalFont, *this);
    activeEditor.reset();

    openFlag = true;
}

void ComputerTerminal::close() {
    StopMusicStream(computerSound);
    activeEditor.reset();
    commandPrompt.reset();
    parser.reset();
    openFlag = false;
}

bool ComputerTerminal::isOpen() const {
    return openFlag;
}

void ComputerTerminal::update() {
    if (!openFlag) return;

    UpdateMusicStream(computerSound);

    if (IsKeyPressed(KEY_ESCAPE)) {
        close();
        return;
    }

    if (activeEditor) {
        activeEditor->update();

        if (activeEditor->isFinished()) {
            Room* room = gameState.currentRoom();
            if (room) {
                for (auto& pf : puzzleFiles) {
                    std::cout << "saved: " << pf.saved << "\n";
                    std::cout << "typingSavedAnswers size: " 
                            << pf.typingSavedAnswers.size() << "\n";
                    for (auto& a : pf.typingSavedAnswers)
                        std::cout << "  answer: '" << a << "'\n";
                    pf.syncToPuzzleState(room->puzzle);
                }
            }

            activeEditor.reset();
        }

        return;
    }

    if (!commandPrompt) return;

    commandPrompt->update();

    // get the last result from commandPrompt somehow
        if (commandPrompt->wantsToExit()) {
            close();
            return;
        }

    if (commandPrompt->wantsToOpenFile()) {
        std::string fname = commandPrompt->getRequestedOpenFile();
        commandPrompt->clearOpenRequest();

        for (auto& pf : puzzleFiles) {
            if (pf.name == fname) {
                if (pf.type == FileType::Dropdwn) {
                    activeEditor = std::make_unique<Dropdwn>(&pf, terminalFont, *this);
                } else {
                    activeEditor = std::make_unique<Typing>(&pf, terminalFont, *this);
                }
                break;
            }
        }
    }
}

void ComputerTerminal::draw() {
    if (!openFlag) return;

    if (activeEditor) {
        activeEditor->draw();
    } else if (commandPrompt) {
        commandPrompt->draw();
    }
}

void ComputerTerminal::playTypingSound() {
    int index = GetRandomValue(0, 3);
    PlaySound(typingSounds[index]);
}
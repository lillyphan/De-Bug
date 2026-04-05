#ifndef COMPUTERTERMINAL_H
#define COMPUTERTERMINAL_H

#include "GameState.h"
#include "CommandParser.h"
#include "terminal/PuzzleFile.h"
#include "terminal/CommandPrompt.h"
#include "terminal/Terminal.h"

#include <memory>
#include <string>
#include <vector>

class ComputerTerminal {
public:
    ComputerTerminal();

    void open(const std::string& roomId = "room_01");
    void close();
    bool isOpen() const;

    GameState& getGameState() { return gameState; }

    Font terminalFont;

    void update();
    void draw();

private:
    void rebuildPuzzleFiles();

    GameState gameState;
    std::vector<PuzzleFile> puzzleFiles;

    std::unique_ptr<CommandParser> parser;
    std::unique_ptr<CommandPrompt> commandPrompt;
    std::unique_ptr<Terminal> activeEditor;

    bool openFlag;
};

#endif
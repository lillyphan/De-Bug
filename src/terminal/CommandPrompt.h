#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#include "Terminal.h"
#include "PuzzleFile.h"
#include "core/headers/CommandParser.h"
#include <string>
#include <vector>
#include "raylib.h"

class ComputerTerminal;  // forward declare

// -------------------------------------------------------
// CommandPrompt — the CLI terminal window where users can run commands
// -------------------------------------------------------
class CommandPrompt : public Terminal {
private:
    std::vector<PuzzleFile>* files;
    CommandParser&           parser;
    Font terminalFont;

    std::vector<std::string> log;
    std::string              input;
    bool                     finished;
    std::string              requestedOpenFile;
    bool exitRequested;

    //helper
    std::vector<std::string> tokenize(const std::string& text) const;
    bool        isValidCommand(const std::string& command) const;
    PuzzleFile* findFile(const std::string& filename);
    void        processInput(const std::string& text);

public:
    //use CommandParser, should have already constructed that with GameState
    CommandPrompt(CommandParser& parser, std::vector<PuzzleFile>*  files, Font font, ComputerTerminal& terminal);

    void update()          override;
    void draw()            override;
    bool isFinished() const override;
    bool wantsToExit() const;

    bool        wantsToOpenFile()      const;
    std::string getRequestedOpenFile() const;
    void        clearOpenRequest();
    void        addLine(const std::string& line);
    ComputerTerminal& terminal;
};

#endif
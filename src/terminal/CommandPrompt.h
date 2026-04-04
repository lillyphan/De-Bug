#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#include "Terminal.h"
#include "PuzzleFile.h"
#include <string>
#include <vector>

class CommandPrompt : public Terminal {
private:
    std::string prompt;
    std::string hint;
    std::vector<std::string> availableCommands;
    std::vector<PuzzleFile>* files;

    std::vector<std::string> log;
    std::string input;
    bool finished;

    std::string requestedOpenFile;

    std::vector<std::string> tokenize(const std::string& text) const;
    bool isValidCommand(const std::string& command) const;
    PuzzleFile* findFile(const std::string& filename);
    bool checkFileCorrectness(const PuzzleFile& file) const;
    void showHelp();
    void processInput(const std::string& text);

public:
    CommandPrompt(const std::string& prompt,
                  const std::string& hint,
                  const std::vector<std::string>& availableCommands,
                  std::vector<PuzzleFile>* files);

    void update() override;
    void draw() override;
    bool isFinished() const override;

    bool wantsToOpenFile() const;
    std::string getRequestedOpenFile() const;
    void clearOpenRequest();

    void addLine(const std::string& line);
};

#endif
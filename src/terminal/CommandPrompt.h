#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#include "Terminal.h"
#include "PuzzleFile.h"
#include <string>
#include <vector>

class CommandPrompt : public Terminal {
private:
    std::string prompt;
    std::vector<std::string> availableCommands;
    std::vector<PuzzleFile>* files;

    std::vector<std::string> log;
    std::string input;
    bool finished;

    std::string requestedOpenFile;
    std::string requestedRunFile;

    std::vector<std::string> tokenize(const std::string& text) const;
    bool isValidCommand(const std::string& command) const;
    PuzzleFile* findFile(const std::string& filename);
    bool checkFileCorrectness(const PuzzleFile& file) const;
    void processInput(const std::string& text);

public:
    CommandPrompt(const std::string& prompt,
                  const std::vector<std::string>& availableCommands,
                  std::vector<PuzzleFile>* files);

    void update() override;
    void draw() override;
    bool isFinished() const override;

    bool wantsToOpenFile() const;
    std::string getRequestedOpenFile() const;
    void clearOpenRequest();

    bool wantsToRunFile() const;
    std::string getRequestedRunFile() const;
    void clearRunRequest();

    void addLine(const std::string& line);
};

#endif
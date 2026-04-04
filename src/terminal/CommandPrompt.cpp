#include "CommandPrompt.h"
#include "raylib.h"
#include <sstream>
#include <algorithm>

CommandPrompt::CommandPrompt(const std::string& prompt,
                             const std::vector<std::string>& availableCommands,
                             std::vector<PuzzleFile>* files)
    : prompt(prompt),
      availableCommands(availableCommands),
      files(files),
      finished(false),
      requestedOpenFile(""),
      requestedRunFile("")
{
    log.push_back(prompt);
    log.push_back("Type help for commands.");
}

std::vector<std::string> CommandPrompt::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

bool CommandPrompt::isValidCommand(const std::string& command) const {
    return std::find(availableCommands.begin(), availableCommands.end(), command) != availableCommands.end();
}

PuzzleFile* CommandPrompt::findFile(const std::string& filename) {
    if (!files) return nullptr;

    for (auto& file : *files) {
        if (file.name == filename) {
            return &file;
        }
    }
    return nullptr;
}

bool CommandPrompt::checkFileCorrectness(const PuzzleFile& file) const {
    if (!file.saved) return false;

    if (file.type == FileType::Typing) {
        if (file.typingSavedAnswers.size() != file.typingSolutions.size()) return false;

        for (int i = 0; i < (int)file.typingSolutions.size(); i++) {
            if (file.typingSavedAnswers[i] != file.typingSolutions[i]) {
                return false;
            }
        }
        return true;
    }

    if (file.type == FileType::Dropdwn) {
        if (file.dropdownSavedIndices.size() != file.dropdownSolutions.size()) return false;

        for (int i = 0; i < (int)file.dropdownSolutions.size(); i++) {
            int selected = file.dropdownSavedIndices[i];
            if (selected < 0 || selected >= (int)file.dropdownOptions[i].size()) {
                return false;
            }

            if (file.dropdownOptions[i][selected] != file.dropdownSolutions[i]) {
                return false;
            }
        }
        return true;
    }

    return false;
}

void CommandPrompt::processInput(const std::string& text) {
    log.push_back("> " + text);

    auto tokens = tokenize(text);

    if (tokens.empty()) {
        return;
    }

    if (!isValidCommand(tokens[0])) {
        log.push_back("Unknown command; type \"help\" for commands.");
        return;
    }

    if (tokens[0] == "help") {
        log.push_back("Available commands:");
        for (const auto& cmd : availableCommands) {
            log.push_back(" - " + cmd);
        }
        return;
    }

    if (tokens[0] == "ls") {
        if (!files || files->empty()) {
            log.push_back("No files available.");
            return;
        }

        for (const auto& file : *files) {
            log.push_back(file.name);
        }
        return;
    }

    if (tokens[0] == "nano") {
        if (tokens.size() < 2) {
            log.push_back("Usage: nano <file>");
            return;
        }

        PuzzleFile* file = findFile(tokens[1]);
        if (!file) {
            log.push_back("File not found.");
            return;
        }

        requestedOpenFile = tokens[1];
        finished = true;
        return;
    }

    if (tokens[0] == "run") {
        if (tokens.size() < 2) {
            log.push_back("Usage: run <file>");
            return;
        }

        PuzzleFile* file = findFile(tokens[1]);
        if (!file) {
            log.push_back("File not found.");
            return;
        }

        if (!file->saved) {
            log.push_back("You must save the file before running it.");
            return;
        }

        if (checkFileCorrectness(*file)) {
            log.push_back(file->name + " ran successfully.");
        } else {
            log.push_back(file->name + " is still incorrect.");
        }

        return;
    }
}

void CommandPrompt::update() {
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126) {
            input += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !input.empty()) {
        input.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER) && !input.empty()) {
        processInput(input);
        input.clear();
    }
}

void CommandPrompt::draw() {
    DrawRectangle(20, 20, 960, 660, BLACK);
    DrawRectangleLines(20, 20, 960, 660, GREEN);

    int y = 40;
    int start = (log.size() > 25) ? (int)log.size() - 25 : 0;

    for (int i = start; i < (int)log.size(); i++) {
        DrawText(log[i].c_str(), 40, y, 22, GREEN);
        y += 24;
    }

    DrawText(("> " + input).c_str(), 40, 640, 24, GREEN);
}

bool CommandPrompt::isFinished() const {
    return finished;
}

bool CommandPrompt::wantsToOpenFile() const {
    return !requestedOpenFile.empty();
}

std::string CommandPrompt::getRequestedOpenFile() const {
    return requestedOpenFile;
}

void CommandPrompt::clearOpenRequest() {
    requestedOpenFile.clear();
    finished = false;
}

bool CommandPrompt::wantsToRunFile() const {
    return !requestedRunFile.empty();
}

std::string CommandPrompt::getRequestedRunFile() const {
    return requestedRunFile;
}

void CommandPrompt::clearRunRequest() {
    requestedRunFile.clear();
}

void CommandPrompt::addLine(const std::string& line) {
    log.push_back(line);
}
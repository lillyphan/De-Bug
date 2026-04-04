#include "CommandPrompt.h"
#include "raylib.h"
#include <sstream>
#include <algorithm>

CommandPrompt::CommandPrompt(const std::string& prompt,
                             const std::string& hint,
                             const std::vector<std::string>& availableCommands,
                             std::vector<PuzzleFile>* files)
    : prompt(prompt),
      hint(hint),
      availableCommands(availableCommands),
      files(files),
      finished(false),
      requestedOpenFile("")
{
    log.push_back(prompt);
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
        for (int i = 0; i < (int)file.typingSolutions.size(); i++) {
            if (file.typingSavedAnswers[i] != file.typingSolutions[i]) {
                return false;
            }
        }
        return true;
    }

    if (file.type == FileType::Dropdwn) {
        for (int i = 0; i < (int)file.dropdownSolutions.size(); i++) {
            int idx = file.dropdownSavedIndices[i];
            if (file.dropdownOptions[i][idx] != file.dropdownSolutions[i]) {
                return false;
            }
        }
        return true;
    }

    return false;
}

void CommandPrompt::showHelp() {
    log.push_back("Available commands:");

    for (const auto& cmd : availableCommands) {
        if (cmd == "help") {
            log.push_back("help - show available commands");
        }
        else if (cmd == "ls") {
            log.push_back("ls - list files");
        }
        else if (cmd == "nano") {
            log.push_back("nano <file> - open file");
        }
        else if (cmd == "run") {
            log.push_back("run <file> - check correctness");
        }
        else if (cmd == "hint") {
            log.push_back("hint - show a hint");
        }
        else {
            log.push_back(cmd);
        }
    }
}

void CommandPrompt::processInput(const std::string& text) {
    log.push_back("> " + text);

    auto tokens = tokenize(text);

    if (tokens.empty()) return;

    if (!isValidCommand(tokens[0])) {
        log.push_back("Unknown command; type \"help\".");
        return;
    }

    if (tokens[0] == "help") {
        showHelp();
        return;
    }

    if (tokens[0] == "hint") {
        log.push_back("Hint: " + hint);
        return;
    }

    if (tokens[0] == "ls") {
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
            log.push_back("You must save before running.");
            return;
        }

        if (checkFileCorrectness(*file)) {
            log.push_back(file->name + " ran successfully.");
        } else {
            log.push_back(file->name + " is incorrect.");
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

void CommandPrompt::addLine(const std::string& line) {
    log.push_back(line);
}
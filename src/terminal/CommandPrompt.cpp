#include "CommandPrompt.h"
#include "raylib.h"
#include <sstream>
#include <algorithm>

CommandPrompt::CommandPrompt(CommandParser& parser, std::vector<PuzzleFile>* files, Font font)
    : parser(parser),
      files(files),
      terminalFont(font),
      finished(false),
      exitRequested(false),
      requestedOpenFile("")
      
{
    //show the room prompt on open
    Room* room = parser.currentRoom();
    if (room)
        log.push_back(room->puzzle.prompt);
}

// -------------------------------------------------------
// Input helpers
// -------------------------------------------------------
std::vector<std::string> CommandPrompt::tokenize(const std::string& text) const
{
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string token;
    while (iss >> token)
        tokens.push_back(token);
    return tokens;
}

bool CommandPrompt::isValidCommand(const std::string& command) const
{
    Room* room = parser.currentRoom();
    if (!room) return false;
    auto& cmds = room->puzzle.availableCommands;
    return std::find(cmds.begin(), cmds.end(), command) != cmds.end();
}

PuzzleFile* CommandPrompt::findFile(const std::string& filename)
{
    if (!files) return nullptr;
    for (auto& f : *files)
        if (f.name == filename) return &f;
    return nullptr;
}

bool CommandPrompt::wantsToExit() const { return exitRequested; }

// -------------------------------------------------------
// Command processing, routes built-in file commands
// -------------------------------------------------------
void CommandPrompt::processInput(const std::string& text)
{
    log.push_back("> " + text);
    auto tokens = tokenize(text);
    if (tokens.empty()) return;

    if (!isValidCommand(tokens[0])) {
        log.push_back("Unknown command; type \"help\".");
        return;
    }

    //ls
    if (tokens[0] == "ls") {
        if (!files || files->empty()) {
            log.push_back("No files.");
            return;
        }
        for (auto& f : *files)
            log.push_back(f.name);
        return;
    }

    //nano
    if (tokens[0] == "nano") {
        if (tokens.size() < 2) {
            log.push_back("Usage: nano <file>");
            return;
        }
        PuzzleFile* f = findFile(tokens[1]);
        if (!f) {
            log.push_back("File not found: " + tokens[1]);
            return;
        }
        requestedOpenFile = tokens[1];
        finished = true;
        return;
    }

    //run
    if (tokens[0] == "run") {
        if (tokens.size() < 2) {
            log.push_back("Usage: run <file>");
            return;
        }
        PuzzleFile* f = findFile(tokens[1]);
        if (!f) {
            log.push_back("File not found: " + tokens[1]);
            return;
        }
        if (!f->saved) {
            log.push_back("Save the file first.");
            return;
        }

        //syncthe saved answers into PuzzleState and let the CommandParser validate
        Room* room = parser.currentRoom();
        if (room) f->syncToPuzzleState(room->puzzle);

        //if they "run" with no editor code (aKA answers already synced into PuzzleState)
        CommandResult result = parser.submit("run", {}, "");
        for (auto& line : result.lines)
            log.push_back(line);

        if (result.solved)
            log.push_back("Correct! The door is now unlocked.");
        else if (!result.lines.empty())
            ;// error message already pushed
        else
            log.push_back(f->name + " output is incorrect.");

        return;
    }

    //CommandParser stuff
    CommandResult result = parser.submit(tokens[0], {}, "");
    if (result.exit) {
        exitRequested = true;
        return;
    }

    if (result.clear) {
        log.clear();
    } else {
        for (auto& line : result.lines)
            log.push_back(line);
    }
}

// -------------------------------------------------------
// Terminal interface
// -------------------------------------------------------
void CommandPrompt::update()
{
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126)
            input += (char)key;
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !input.empty())
        input.pop_back();

    if (IsKeyPressed(KEY_ENTER) && !input.empty()) {
        processInput(input);
        input.clear();
    }
}

void CommandPrompt::draw()
{
    DrawRectangle(20, 20, 960, 660, BLACK);
    DrawRectangleLines(20, 20, 960, 660, GREEN);

    int y     = 40;
    int start = (log.size() > 25) ? (int)log.size() - 25 : 0;

    for (int i = start; i < (int)log.size(); i++) {
        DrawTextEx(this->terminalFont, log[i].c_str(), (Vector2){40, (float)y}, 22, 1, GREEN);
        y += 24;
    }

    DrawTextEx(terminalFont, (" > " + input).c_str(), (Vector2){40, 640}, 24, 1, GREEN);
}

bool CommandPrompt::isFinished() const { return finished; }

bool CommandPrompt::wantsToOpenFile() const { return !requestedOpenFile.empty(); }

std::string CommandPrompt::getRequestedOpenFile() const { return requestedOpenFile; }

void CommandPrompt::clearOpenRequest()
{
    requestedOpenFile.clear();
    finished = false;
}

void CommandPrompt::addLine(const std::string& line) { log.push_back(line); }
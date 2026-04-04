#include "raylib.h"
#include "PuzzleFile.h"
#include "CommandPrompt.h"
#include "Dropdwn.h"
#include "Typing.h"

#include <memory>
#include <vector>
#include <string>

int main() {
    InitWindow(1000, 700, "De-Bug Test");
    SetTargetFPS(60);

    // Hardcoded version of:
    //
    // Why does it keep crashing!?!?!? Fix it!!!
    // crash.py
    // Maybe look at the files available? Edit the file?
    // ls nano run
    // dp
    // 3
    // while (true):
    //        if goingToCrash:
    //                  _
    // crash !crash
    // !crash

    std::vector<PuzzleFile> files;

    PuzzleFile crashFile;
    crashFile.name = "crash.py";
    crashFile.type = FileType::Dropdwn;
    crashFile.starterCodeLines = {
        "while (true):",
        "       if goingToCrash:",
        "                 _"
    };
    crashFile.dropdownOptions = {
        {"crash", "!crash"}
    };
    crashFile.dropdownSolutions = {
        "!crash"
    };
    crashFile.dropdownSavedIndices = {
        0
    };
    crashFile.saved = false;

    files.push_back(crashFile);

    std::vector<std::string> commands = {
        "help", "ls", "nano", "run"
    };

    std::unique_ptr<Terminal> currentScreen =
        std::make_unique<CommandPrompt>(
            "Why does it keep crashing!?!?!? Fix it!!!",
            commands,
            &files
        );

    while (!WindowShouldClose()) {
        currentScreen->update();

        BeginDrawing();
        ClearBackground(BLACK);
        currentScreen->draw();
        EndDrawing();

        // If command prompt requests opening a file, switch screens
        if (auto* prompt = dynamic_cast<CommandPrompt*>(currentScreen.get())) {
            if (prompt->wantsToOpenFile()) {
                std::string filename = prompt->getRequestedOpenFile();
                prompt->clearOpenRequest();

                PuzzleFile* selectedFile = nullptr;
                for (auto& file : files) {
                    if (file.name == filename) {
                        selectedFile = &file;
                        break;
                    }
                }

                if (selectedFile != nullptr) {
                    if (selectedFile->type == FileType::Dropdwn) {
                        currentScreen = std::make_unique<Dropdwn>(selectedFile);
                    } else if (selectedFile->type == FileType::Typing) {
                        currentScreen = std::make_unique<Typing>(selectedFile);
                    }
                }
            }
        }
        // If an editor is finished, return to command prompt
        else if (currentScreen->isFinished()) {
            currentScreen = std::make_unique<CommandPrompt>(
                "Why does it keep crashing!?!?!? Fix it!!!",
                commands,
                &files
            );
        }
    }

    CloseWindow();
    return 0;
}
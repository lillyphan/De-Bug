#ifndef PUZZLEFILE_H
#define PUZZLEFILE_H

#include <string>
#include <vector>

enum class FileType {
    Typing,
    Dropdwn
};

struct PuzzleFile {
    std::string name;
    FileType type;

    // Shared starter code lines
    std::vector<std::string> starterCodeLines;

    // Typing puzzle data
    std::vector<std::string> typingSolutions;
    std::vector<std::string> typingSavedAnswers;

    // Dropdown puzzle data
    std::vector<std::vector<std::string>> dropdownOptions;
    std::vector<std::string> dropdownSolutions;
    std::vector<int> dropdownSavedIndices;

    bool saved = false;
};

#endif
#ifndef PUZZLEFILE_H
#define PUZZLEFILE_H

#include <string>
#include <vector>
#include "core/headers/GameState.h"
#include "core/headers/PuzzleEngine.h"
#include <sstream>

// -------------------------------------------------------
// PuzzleFile, bridges PuzzleState to the terminal UI classes
// -------------------------------------------------------

enum class FileType {
    Typing,
    Dropdwn
};

struct PuzzleFile {
    std::string name;
    FileType type;

    //starter code split into lines
    std::vector<std::string> starterCodeLines;

    //typing puzzle data
    std::vector<std::string> typingSolutions;
    std::vector<std::string> typingSavedAnswers;

    //dropdown puzzle data
    std::vector<std::vector<std::string>> dropdownOptions;
    std::vector<std::string>              dropdownSolutions;
    std::vector<int>                      dropdownSavedIndices;

    bool saved = false;

    // -------------------------------------------------------
    // build a PuzzleFile from a PuzzleState
    // we can call this after PuzzleLoader has populated GameState
    // -------------------------------------------------------
    static PuzzleFile fromPuzzleState(const PuzzleState& ps, const std::string& filename)
    {
        PuzzleFile pf;
        pf.name  = filename;
        pf.saved = false;

        //split startingCode into lines for Dropdwn/Typing to iterate
        std::istringstream stream(ps.startingCode);
        std::string line;
        while (std::getline(stream, line))
            pf.starterCodeLines.push_back(line);

        if (ps.type == PuzzleType::Dropdown) {
            pf.type = FileType::Dropdwn;

            //each dropdown in PuzzleState becomes one set of options
            for (auto& d : ps.dropdowns)
                pf.dropdownOptions.push_back(d.options);

            pf.dropdownSolutions   = ps.correctSequence;
            pf.dropdownSavedIndices = std::vector<int>(ps.dropdowns.size(), 0);

        } else {
            pf.type = FileType::Typing;

            //for typing puzzles the solution is gonna marked by a single string —
            //wrap it in a vector to match the per-blank structure for typing class
            pf.typingSolutions    = { ps.correctCode };
            pf.typingSavedAnswers = { "" };
        }

        return pf;
    }

    // -------------------------------------------------------
    // write saved answers back into PuzzleState so PuzzleEngine can validate
    // call this after the player saves anything in Dropdwn or Typing windows
    // -------------------------------------------------------
    void syncToPuzzleState(PuzzleState& ps) const
    {
        if (type == FileType::Typing) {
            //put the saved answers back into a single playerCode string
            ps.playerCode = typingSavedAnswers.empty() ? "" : typingSavedAnswers[0];

        } else {
            //write selected indices back into PuzzleState dropdowns
            for (int i = 0; i < (int)dropdownSavedIndices.size() &&
                             i < (int)ps.dropdowns.size(); i++) {
                ps.dropdowns[i].selectedIndex = dropdownSavedIndices[i];
            }
        }
    }

    // -------------------------------------------------------
    // validate using puzzle engine
    // -------------------------------------------------------
    bool checkCorrectness(PuzzleState& ps, PuzzleEngine& engine) const
    {
        if (!saved) return false;
        syncToPuzzleState(ps);
        return engine.validate(ps, ps.type);
    }
};

#endif
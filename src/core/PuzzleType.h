#pragma once
 
enum class PuzzleType {
    Code,      // player edits a code snippet, matched against correctCode
    Dropdown,  // player selects a sequence of commands, matched against correctSequence
};
 
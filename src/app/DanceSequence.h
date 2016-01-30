#pragma once
#include "Globals.h"

class DanceMove;
class DanceSequence
{
public:

    DanceSequence() {};
    virtual ~DanceSequence() {};

    void Init(int in_difficultyLevel);
    
private:

    DanceMoveVect           m_moves;

    int                     GetNbMovesForDifficulty(int in_difficulty);
    DanceMove               GetDanceMoveForDifficulty(int in_difficulty);

    static DanceMoveVect    s_possibleDanceMoves;

};
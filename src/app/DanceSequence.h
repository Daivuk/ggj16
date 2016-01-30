#pragma once
#include "Globals.h"
#include "DanceMove.h"

class Fireplace;
class DanceSequence
{
public:

    DanceSequence() {};
    virtual ~DanceSequence() {};

    void Init(int in_difficultyLevel, Fireplace* in_fireplace);
    void Update();
    
private:


    float                   m_timeActive = 0;
    float                   m_maxTime = 0;
    DanceMove*              m_currentDanceMove = nullptr;
    DanceMoveVect           m_moves;
    Fireplace*              m_fireplace = nullptr;

    int                     GetNbMovesForDifficulty(int in_difficulty);
    DanceMove               GetDanceMoveForDifficulty(int in_difficulty);
    void                    ShowNextMove();

    static DanceMoveVect    s_possibleDanceMoves;

};
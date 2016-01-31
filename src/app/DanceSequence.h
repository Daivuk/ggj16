#pragma once
#include "Globals.h"
#include "DanceMove.h"
#include "View.h"

class Fireplace;
class DanceSequence
{
public:

    DanceSequence() {};
    virtual ~DanceSequence();

    void Init(int in_difficultyLevel, Fireplace* in_fireplace, seed::View* in_container);
    bool Update(); // returns true when the current dance move expired
    void Skip();

    bool PlayerNeedsToComplete(int in_playerControllerIndex);
    bool IsNailed(DanceMoveButtonVect& in_playerInputs, int in_playerControllerIndex);

    static DanceMoveButtonVect& GetPossibleButtons() { return s_possibleButtons;  }
        
private:


    float                   m_timeActive = 0;
    float                   m_maxTime = 0;
    DanceMove*              m_currentDanceMove = nullptr;
    int                     m_currentDanceMoveIndex = 0;
    DanceMoveVect           m_moves;
    Fireplace*              m_fireplace = nullptr;
    seed::View*              m_container = nullptr;

    int                     GetNbButtonsPerDanceMoveForDifficulty(int in_difficulty);
    DanceMove               GetDanceMove(int in_nbButtons);
    void                    ShowNextMove();

    bool                    m_waitingForMoveToHide = false;

    static DanceMoveButtonVect    s_possibleButtons;

};
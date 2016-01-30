#include "DanceSequence.h"
#include "Globals.h"
#include "Entity.h"
#include "Fireplace.h"

DanceMoveVect DanceSequence::s_possibleDanceMoves =
{
    DanceMove({ OABtn }),
    DanceMove({ OBBtn }),
    DanceMove({ OXBtn }),
    DanceMove({ OYBtn }),
    DanceMove({ OLTBtn }),
    DanceMove({ OLBBtn }),
    DanceMove({ ORTBtn }),
    DanceMove({ ORBBtn }),

    DanceMove({ OABtn, OBBtn }),
    DanceMove({ OABtn, OXBtn }),
    DanceMove({ OABtn, OYBtn }),
    DanceMove({ OABtn, OLTBtn }),
    DanceMove({ OABtn, OLBBtn }),
    DanceMove({ OABtn, ORTBtn }),
    DanceMove({ OABtn, ORBBtn }),

    DanceMove({ OBBtn, OABtn }),
    DanceMove({ OBBtn, OXBtn }),
    DanceMove({ OBBtn, OYBtn }),
    DanceMove({ OBBtn, OLTBtn }),
    DanceMove({ OBBtn, OLBBtn }),
    DanceMove({ OBBtn, ORTBtn }),
    DanceMove({ OBBtn, ORBBtn }),

    DanceMove({ OXBtn, OABtn }),
    DanceMove({ OXBtn, OBBtn }),
    DanceMove({ OXBtn, OYBtn }),
    DanceMove({ OXBtn, OLTBtn }),
    DanceMove({ OXBtn, OLBBtn }),
    DanceMove({ OXBtn, ORTBtn }),
    DanceMove({ OXBtn, ORBBtn }),

    DanceMove({ OYBtn, OABtn }),
    DanceMove({ OYBtn, OBBtn }),
    DanceMove({ OYBtn, OXBtn }),
    DanceMove({ OYBtn, OLTBtn }),
    DanceMove({ OYBtn, OLBBtn }),
    DanceMove({ OYBtn, ORTBtn }),
    DanceMove({ OYBtn, ORBBtn }),

    DanceMove({ OYBtn, OABtn }),
    DanceMove({ OYBtn, OBBtn }),
    DanceMove({ OYBtn, OXBtn }),
    DanceMove({ OYBtn, OLTBtn }),
    DanceMove({ OYBtn, OLBBtn }),
    DanceMove({ OYBtn, ORTBtn }),
    DanceMove({ OYBtn, ORBBtn }),

};


void DanceSequence::Init(int in_difficultyLevel, Fireplace* in_fireplace, seed::View* in_container)
{
    m_container = in_container;
    m_timeActive = 0;
    m_maxTime = 5.f - ((float)(in_difficultyLevel-1) * .2f);
    m_fireplace = in_fireplace;
    const int nbMoves = GetNbMovesForDifficulty(in_difficultyLevel);
    for (int i = 0; i < nbMoves; ++i)
    {
        m_moves.push_back(GetDanceMoveForDifficulty(in_difficultyLevel));
    }
    ShowNextMove();
}

void DanceSequence::Update()
{
    m_timeActive += ODT;
    if (m_timeActive >= m_maxTime)
    {
        // time to switch to the next move
        ShowNextMove();
    }
}

void DanceSequence::ShowNextMove()
{
    if (m_currentDanceMove)
    {
        m_currentDanceMove->Hide();
    }
    m_timeActive = 0;
    if (m_currentDanceMoveIndex >= (int)m_moves.size())
    {
        m_currentDanceMoveIndex = 0;
    }
    m_currentDanceMove = &(m_moves[m_currentDanceMoveIndex]);
    m_currentDanceMove->Show(m_fireplace, m_container);
    m_currentDanceMoveIndex++;
}

DanceMove DanceSequence::GetDanceMoveForDifficulty(int in_difficulty)
{
    int lowerRange = in_difficulty - 3;
    lowerRange = lowerRange < 0 ? 0 : lowerRange;
    int higherRange = in_difficulty;

    int moveIndex = onut::randi(lowerRange, higherRange);
    return s_possibleDanceMoves[moveIndex];
}

int DanceSequence::GetNbMovesForDifficulty(int in_difficulty)
{
    if (in_difficulty < 5)
    {
        return 5;
    }
    if (in_difficulty < 10)
    {
        return 10;
    }
    if (in_difficulty < 15)
    {
        return 15;
    }
    return 20;
}

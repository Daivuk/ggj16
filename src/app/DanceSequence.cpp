#include "DanceSequence.h"
#include "Globals.h"
#include "Entity.h"
#include "Fireplace.h"
#include "GameView.h"

DanceMoveButtonVect DanceSequence::s_possibleButtons =
{
    OABtn,
    OBBtn,
    OXBtn,
    OYBtn,
};


void DanceSequence::Init(int in_difficultyLevel, Fireplace* in_fireplace, seed::View* in_container)
{
    m_container = in_container;
    m_timeActive = 0;
    m_maxTime = 3.f;
    m_fireplace = in_fireplace;
    const int nbButtons = GetNbButtonsPerDanceMoveForDifficulty(in_difficultyLevel);
    const int nbDanceMoves = 10;
    for (int i = 0; i < nbDanceMoves; ++i)
    {
        m_moves.push_back(GetDanceMove(nbButtons));
    }
    ShowNextMove();
}

void DanceSequence::Skip()
{
    m_timeActive = m_maxTime;
}

bool DanceSequence::Update()
{
    m_timeActive += ODT;
    if (m_timeActive >= m_maxTime)
    {
        // time to switch to the next move
        ShowNextMove();
        return true;
    }
    return false;
}

bool DanceSequence::PlayerNeedsToComplete(int in_playerControllerIndex)
{
    if (m_currentDanceMove)
    {
        return m_currentDanceMove->PlayerNeedsToComplete(in_playerControllerIndex);
    }
    return false;
}

bool DanceSequence::IsNailed(DanceMoveButtonVect& in_playerInputs, int in_playerControllerIndex)
{
    if (m_currentDanceMove)
    {
        return m_currentDanceMove->IsNailed(in_playerInputs, in_playerControllerIndex);
    }
    return false;
}

void DanceSequence::ShowNextMove()
{
    if (m_currentDanceMove)
    {
        if (!m_waitingForMoveToHide)
        {
            m_currentDanceMove->Hide();
            m_waitingForMoveToHide = true;
            return;
        }
        else
        {
            if (m_currentDanceMove->IsDoneHiding())
            {
                m_currentDanceMove = nullptr;
                m_waitingForMoveToHide = false;
            }
            else
            {
                return;
            }
        }
    }

    m_timeActive = 0;
    if (m_currentDanceMoveIndex >= (int)m_moves.size() - 1)
    {
        m_currentDanceMoveIndex = 0;
        RandomizeVector(m_moves);
    }
    m_currentDanceMove = &(m_moves[m_currentDanceMoveIndex]);
    m_currentDanceMove->Show(m_maxTime, m_fireplace, m_container);
    m_currentDanceMoveIndex++;
}

DanceMove DanceSequence::GetDanceMove(int in_nbButtons)
{
    const int maxPossibleButtons = (int)s_possibleButtons.size() - 1;
    DanceMoveButtonVect buttons;
    for (int i = 0; i < in_nbButtons; ++i)
    {
        buttons.push_back(s_possibleButtons[onut::randi(maxPossibleButtons)]);
    }
    return DanceMove(buttons);
}

int DanceSequence::GetNbButtonsPerDanceMoveForDifficulty(int in_difficulty)
{
    return in_difficulty;
}

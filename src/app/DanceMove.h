#pragma once
#include "onut.h"
#include "SeedGlobals.h"
#include "Globals.h"
#include "Entity.h"

class DanceMove : public seed::Node
{
public:
    DanceMove(const DanceMoveButtonVect& in_buttons);
    virtual ~DanceMove() {};

    void Show(float in_time, Entity* in_entity, seed::View* in_container);
    void Hide();
    bool IsDoneHiding();

    bool PlayerNeedsToComplete(int in_playerControllerIndex);
    bool IsNailed(DanceMoveButtonVect& in_playerInputs, int in_playerControllerIndex);
    
private:
    DanceMoveButtonVect     m_buttons;  // button that needs to be pressed to nail this move!
    seed::SpriteVect        m_displayedButtons;
    seed::View*             m_container = nullptr;
    std::vector<int>        m_playerCompleted;  // indexes of players who completed this dance move sequence

    // actually displayed sprites

    seed::Sprite* CreateSpriteForButton(onut::GamePad::eGamePad in_button);
    bool IsSequenceFound(DanceMoveButtonVect& in_playerInputs, int startIndex);
    
};
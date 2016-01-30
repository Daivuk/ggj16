#pragma once
#include "onut.h"
#include "SeedGlobals.h"
#include "Globals.h"
#include "Entity.h"

class DanceMove : public Entity
{
public:

    DanceMove(const DanceMoveButtonVect& in_buttons);
    virtual ~DanceMove() {};

    void Show(Entity* in_entity, seed::View* in_container);
    void Hide();

private:

    DanceMoveButtonVect     m_buttons;  // button that needs to be pressed to nail this move!
    seed::SpriteVect        m_displayedButtons;
    seed::View*             m_container = nullptr;

    // actually displayed sprites

    seed::Sprite* CreateSpriteForButton(onut::GamePad::eGamePad in_button);

    //void HideAnimEnded();

};
#pragma once
#include "onut.h"
#include "Globals.h"
#include "Entity.h"

class DanceMove : public Entity
{
public:

    DanceMove(const DanceMoveButtonVect& in_buttons);
    virtual ~DanceMove() {};

private:

    DanceMoveButtonVect     m_buttons;  // button that needs to be pressed to nail this move!

};
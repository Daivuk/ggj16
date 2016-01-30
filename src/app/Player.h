#pragma once
#include "Sprite.h"
#include "View.h"
#include "Globals.h"

#include <string>

class Player
{
public:

    Player();
    virtual ~Player();

    void Init(const Vector2& in_position, seed::View* in_container, seed::Node* in_parent, int in_controllerIndex);
    void Update();
	void OnRender();

    DanceMoveButtonVect&    GetInputSequence() { return m_inputSequence; }
    int                     GetControllerIndex() { return m_controllerIndex; }
    
    void                    OnDanceSequenceSuccess();
    void                    ResetInputSequence();

private:

    seed::View*     m_container = nullptr;
    seed::Sprite*   m_sprite = nullptr;
    seed::Sprite*   m_danceMoveNailed = nullptr;
    int             m_controllerIndex = -1;
    std::string     m_idleAnim;
    Vector2         m_thumb;
    Vector2         m_vel;

    DanceMoveButtonVect m_inputSequence; // accumulated button sequence

    void    UpdateInputs();
    void    UpdateVel();
    void    UpdateSpriteAnim();


};
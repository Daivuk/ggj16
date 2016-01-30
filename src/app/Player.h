#pragma once
#include "Sprite.h"
#include "View.h"
#include "Globals.h"
#include "Entity.h"

#include <string>

class Player : public Entity
{
public:

    Player();
    virtual ~Player();

    void Init(const Vector2& in_position, seed::View* in_container, int in_controllerIndex);
	void OnRender();
    void UpdateEntity() override;

    DanceMoveButtonVect&    GetInputSequence() { return m_inputSequence; }
    int                     GetControllerIndex() { return m_controllerIndex; }
    
    void                    OnDanceSequenceSuccess();
    void                    ResetInputSequence();

    virtual float   GetWidth() const { return .5f; }
    virtual float   GetHeight() const { return .5f; }

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
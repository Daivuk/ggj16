#pragma once
#include "Sprite.h"
#include "View.h"
#include "Globals.h"
#include "Entity.h"
#include "SoundEmitter.h"
#include "Globals.h"

#include <string>

class DancePedestral;

enum class ePlayerDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class PlayerState
{
    IDLE,
    PEDESTRAL,
    ATTACKING,
    CARYING_WOOD,
    CARYING_ROCKS,
};

class Player : public Entity
{
public:

    Player();
    virtual ~Player();

    void Init(const Vector2& in_position, seed::View* in_container, int in_controllerIndex);
    void OnRender();
    void UpdateEntity() override;
    bool HasCarryOn() const { return m_pCarryOn != nullptr; }

    DanceMoveButtonVect&    GetInputSequence() { return m_inputSequence; }
    int                     GetControllerIndex() { return m_controllerIndex; }
    
    void                    OnDanceSequenceSuccess();
    void                    ResetInputSequence();
    void                    OnPedestralLockedIn(DancePedestral* in_pedestral);
    void                    OnPedestralLockCancel();
    void                    DropCarryOn();
    DropType                GiveCarryOn();

    virtual float   GetWidth() const { return .5f; }
    virtual float   GetHeight() const { return .5f; }

private:

    seed::View*     m_container = nullptr;
    seed::Sprite*   m_sprite = nullptr;
    seed::Sprite*   m_slash = nullptr;
    seed::Sprite*   m_danceMoveNailed = nullptr;
    int             m_controllerIndex = -1;
    std::string     m_idleAnim;
    Vector2         m_thumb;
    Vector2         m_vel;
    Entity*         m_pCarryOn = nullptr;
    

    DanceMoveButtonVect m_inputSequence; // accumulated button sequence

    void    UpdateInputs();
    void    UpdateVel();
    void    UpdateSpriteAnim();
    void    UpdatePedestralSnap();

    void    Attack();

    DancePedestral * m_currentDancePedestral = nullptr;

    seed::SoundEmitter* m_drumSoundEmmiter = nullptr;
    seed::SoundEmitter* m_slashSoundEmmiter = nullptr;

    ePlayerDirection m_currentDirection = ePlayerDirection::DOWN;
    PlayerState m_playerState = PlayerState::IDLE;
    OTimer m_stateTimer;
};
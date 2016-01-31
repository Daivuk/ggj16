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
    CARYING_STUFF,
    RECEIVING_DAMAGE,
    DEAD
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
    DropType GetDropType() const;

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

    void AfterDamagePush(const Vector2& in_direction);

    bool    IsAlive() { return m_playerState != PlayerState::DEAD; }

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

    seed::Sprite*   m_healthGauge = nullptr;
    seed::Sprite*   m_healthFill = nullptr;
    

    DanceMoveButtonVect m_inputSequence; // accumulated button sequence

    void    UpdateHealthBar();
    void    UpdateInputs();
    void    UpdateVel();
    void    UpdateSpriteAnim();
    void    UpdatePedestralSnap();
    void    UpdateStoneIndicator();

    void    Attack();
    void    OnDeath();
    void    OnSacrifice();

    DancePedestral * m_currentDancePedestral = nullptr;

    seed::SoundEmitter* m_drumSoundEmmiter = nullptr;
    seed::SoundEmitter* m_slashSoundEmmiter = nullptr;
    seed::Sprite*           m_damageBlood = nullptr;
    seed::Sprite*           m_dottedLine = nullptr;
    seed::SoundEmitter*     m_damageSound = nullptr;
    seed::SoundEmitter*     m_deathSound = nullptr;

    ePlayerDirection m_currentDirection = ePlayerDirection::DOWN;
    PlayerState m_playerState = PlayerState::IDLE;
    PlayerState m_lastState = PlayerState::IDLE;
    OTimer m_stateTimer;

    OAnim<Vector2> m_velPushAnim;

    seed::PhysicsBody* m_physicsBody = nullptr;

    float   m_lastHealthValue = 100.f;
    OAnim<float>    m_healthAnim;

};
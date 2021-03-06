#pragma once

#include "Entity.h"
#include "SoundEmitter.h"


enum class MonsterType
{
    CRAWLER,
};

enum class MonsterState
{
    IDLE,
    GO_TO,
    ATTACK,
    AFTER_DAMAGE_PUSH,
    DEAD
};

class Monster : public Entity
{
public:
    static int count;

    Monster(MonsterType monsterType, seed::View* pView, const Vector2& position);
    virtual ~Monster();

    virtual void Kill();
    virtual void UpdateEntity() override;
    void PathTo(const Vector2& position);
    void Seek();

    std::vector<Vector2> m_path;

    void AfterDamagePush(const Vector2& in_direction);

private:
    void UpdateSpriteAnim(const Vector2& dir);

    seed::View* m_pView = nullptr;
    Vector2 m_targetPos;
    float m_speed = 2.f;
    seed::PhysicsBody* m_pPhysicBody = nullptr;
    bool m_nextUnghost = false;
    seed::Sprite* m_sprite = nullptr;

    MonsterState m_state = MonsterState::IDLE;
    MonsterState m_previousState = MonsterState::IDLE;


    seed::Sprite*           m_damageBlood = nullptr;
    seed::SoundEmitter*     m_damageSound = nullptr;
    seed::SoundEmitter*     m_deathSound = nullptr;
    OAnim<Vector2> m_velPushAnim;
    Vector2                 m_lastAnimDir;
    
    void OnDeath();

    void CheckIfMonsterCanAttack(vector<Entity*>& inOut_attackablePlayers);
    void OnMonsterAttack(vector<Entity*>& in_attackablePlayers);

    float m_attackTimer = 0.f;
};

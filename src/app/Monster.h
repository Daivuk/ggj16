#pragma once

#include "Entity.h"

enum class MonsterType
{
    CRAWLER,
};

enum class MonsterState
{
    IDLE,
    GO_TO,
    ATTACK
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

private:
    seed::View* m_pView = nullptr;
    Vector2 m_targetPos;
    float m_speed = 2.f;
    seed::PhysicsBody* m_pPhysicBody = nullptr;

    MonsterState m_state = MonsterState::IDLE;
};

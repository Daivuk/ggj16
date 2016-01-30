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
    void UpdateSpriteAnim(const Vector2& dir);

    seed::View* m_pView = nullptr;
    Vector2 m_targetPos;
    float m_speed = 2.f;
    seed::PhysicsBody* m_pPhysicBody = nullptr;
    bool m_nextUnghost = false;
    seed::Sprite* m_sprite = nullptr;

    MonsterState m_state = MonsterState::IDLE;
};

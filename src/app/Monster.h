#pragma once

#include "Entity.h"

enum class MonsterType
{
    CRAWLER,
};

class Monster : public Entity
{
public:
    static int count;

    Monster(MonsterType monsterType, seed::View* pView, const Vector2& position);
    virtual ~Monster();

private:
    seed::View* m_pView = nullptr;
};

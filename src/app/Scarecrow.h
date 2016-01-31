#pragma once
#include "Entity.h"

class Scarecrow : public Entity
{
public:
    Scarecrow(seed::View* pView);

    void Place(const Vector2& position);

    virtual bool InflictDamage(float in_damage) override; // return true if the damage killed it

private:
    seed::View* m_pView = nullptr;
    seed::Sprite* m_pSprite = nullptr;
};

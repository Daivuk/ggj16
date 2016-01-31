#pragma once

#include "Entity.h"

class Tree : public Entity
{
public:
    Tree(seed::View *pView, const Vector2& in_position);

    virtual float   GetWidth() const { return 1.f; }
    virtual float   GetHeight() const { return 1.f; }

    virtual bool InflictDamage(float in_damage) override; // return true if the damage killed it

private:
    seed::Sprite* m_pSprite = nullptr;
};

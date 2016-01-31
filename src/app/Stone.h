#pragma once
#include "Entity.h"

class Stone : public Entity
{
public:
    Stone(seed::View* pView);

    virtual float   GetWidth() const { return 1.f; }
    virtual float   GetHeight() const { return 1.f; }

    void Place(const Vector2& position);

private:
    seed::View* m_pView = nullptr;
    seed::Sprite* m_pSprite = nullptr;
};

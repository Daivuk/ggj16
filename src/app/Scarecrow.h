#pragma once
#include "Entity.h"

class Scarecrow : public Entity
{
public:
    Scarecrow(seed::View* pView);

    void Place(const Vector2& position);

private:
    seed::View* m_pView = nullptr;
    seed::Sprite* m_pSprite = nullptr;
};

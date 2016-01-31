#pragma once
#include "Entity.h"

class Stone : public Entity
{
public:
    Stone(seed::View* pView);

private:
    seed::View* m_pView = nullptr;
    seed::Sprite* m_pSprite = nullptr;
};

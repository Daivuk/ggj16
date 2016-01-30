#pragma once
#include "Sprite.h"
#include "View.h"
#include "Entity.h"

#include <string>

class Fireplace : public Entity
{
public:
    Fireplace(seed::View* pView, const Vector2& position);
    virtual ~Fireplace();

    void Grow();
    virtual void UpdateEntity();

private:

    OAnimf m_radiusAnim;
    seed::Sprite* m_sprite = nullptr;
    float m_targetRadius = 0;
    bool m_gameover = false;
};

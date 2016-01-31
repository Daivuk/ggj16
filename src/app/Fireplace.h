#pragma once
#include "Sprite.h"
#include "View.h"
#include "Entity.h"
#include "SoundEmitter.h"
#include "Emitter.h"

#include <string>

class Fireplace : public Entity
{
public:
    Fireplace(seed::View* pView, const Vector2& position);
    virtual ~Fireplace();

    void Grow();
    void GrowToMax();
    virtual void UpdateEntity();

private:

    OAnimf m_radiusAnim;
    float m_targetRadius = 0;
    bool m_gameover = false;

    seed::SoundEmitter*     m_whispers = nullptr;
    seed::Emitter*          m_fireFX1 = nullptr;
    seed::Emitter*          m_fireFX2 = nullptr;
};

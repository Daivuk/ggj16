#pragma once
#include "Sprite.h"
#include "Emitter.h"
#include "View.h"
#include "Entity.h"

#include <string>

class DancePedestral : public Entity
{
public:
    DancePedestral(seed::View* pView, const Vector2& position, int index);
    virtual ~DancePedestral();

    bool m_isOccupied = false;

    void StartEnabledFX();
    void StartActivatedFX();
    void StopFXes();

private:

    seed::Emitter* m_baseFX = nullptr;
    seed::Emitter* m_activatedFX = nullptr;


};

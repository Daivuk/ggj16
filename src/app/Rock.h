#pragma once

#include "Entity.h"

class Rock : public Entity
{
public:
    Rock(seed::View *pView, const Vector2& in_position);

    virtual float   GetWidth() const { return 1.f; }
    virtual float   GetHeight() const { return 1.f; }
};

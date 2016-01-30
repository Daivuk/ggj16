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

private:
};

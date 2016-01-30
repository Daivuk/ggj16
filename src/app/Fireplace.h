#pragma once
#include "Sprite.h"
#include "View.h"

#include <string>

class Fireplace
{
public:
    Fireplace(seed::View* pView, seed::Node* in_parent, const Vector2& position);
    virtual ~Fireplace();

private:
    Vector2 m_position;
};

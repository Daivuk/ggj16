#pragma once
#include "Node.h"
#include "Tile.h"

class Entity : public seed::Node
{
public:
    Entity() {};
    virtual ~Entity() {};

    virtual void UpdateEntity();

    LIST_LINK(Entity) linkTile;

private:
    Vector2 m_lastPosition;
};

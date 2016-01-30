#pragma once
#include "Node.h"
#include "Tile.h"

class Entity : public seed::Node
{
public:
    Entity() {};
    virtual ~Entity() {};

    LIST_LINK(Entity) linkTile;
};

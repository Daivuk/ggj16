#pragma once

#include "List.h"

class Entity;

class Tile
{
public:
    Tile();
    ~Tile();

    void RegisterEntity(Entity* pEntity);

    TList<Entity> *pEntities = nullptr;
    bool isOccupied = false;
};

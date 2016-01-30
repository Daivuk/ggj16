#include "Tile.h"
#include "Entity.h"

Tile::Tile()
{
    pEntities = new TList<Entity>(offsetOf(&Entity::linkTile));
}

Tile::~Tile()
{
    delete pEntities;
}

void Tile::RegisterEntity(Entity* pEntity)
{
    pEntities->InsertTail(pEntity);
}

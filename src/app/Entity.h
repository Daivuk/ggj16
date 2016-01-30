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

    bool Collide() const { return m_collide; }
    float GetCollisionRadius() const { return m_collisionRadius; }

protected:
    bool m_collide = false;
    float m_collisionRadius = .25f;

private:
    bool PerformWallCollisions(const Vector2& from, Vector2& toOut);

    Vector2 m_lastPosition;
};

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

    virtual bool InflictDamage(float in_damage); // return true if the damage killed it
    virtual void RestoreFullHealth() { m_health = 100.f; }
    
protected:
    bool m_collide = false;
    float m_collisionRadius = .25f;
    float   m_health = 100;

private:
    bool PerformWallCollisions(const Vector2& from, Vector2& toOut);

    Vector2 m_lastPosition;
};

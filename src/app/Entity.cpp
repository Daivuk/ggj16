#include "Entity.h"
#include "GameView.h"

void Entity::UpdateEntity()
{
    auto position = GetPosition();
    if (position != m_lastPosition)
    {
        if (PerformWallCollisions(m_lastPosition, position))
        {
            SetPosition(position);
        }

        g_gameView->OnEntityMoved(this);
        m_lastPosition = position;
    }
}

bool Entity::PerformWallCollisions(const Vector2& from, Vector2& toOut)
{
    int xFrom = (int)(onut::min(from.x, toOut.x) - m_collisionRadius);
    int xTo = (int)(onut::max(from.x, toOut.x) + m_collisionRadius);
    int yFrom = (int)(onut::min(from.y, toOut.y) - m_collisionRadius);
    int yTo = (int)(onut::max(from.y, toOut.y) + m_collisionRadius);

    for (int y = yFrom; y <= yTo; ++y)
    {
        for (int x = xFrom; x <= xTo; ++x)
        {
            auto pTile = g_gameView->GetTileAt(x, y);
            bool doCollision = !pTile;
            if (!doCollision && pTile)
            {
                doCollision = pTile->isOccupied;
            }
            if (doCollision)
            {

            }
        }
    }

    return false;
}

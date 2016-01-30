#include "Entity.h"
#include "GameView.h"

void Entity::UpdateEntity()
{
    auto& position = GetPosition();
    if (position != m_lastPosition)
    {
        g_gameView->OnEntityMoved(this);
        m_lastPosition = position;
    }
}

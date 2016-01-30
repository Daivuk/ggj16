#include "Monster.h"
#include "View.h"
#include "Globals.h"
#include "Sprite.h"
#include "GameView.h"
#include "Player.h"
#include "PhysicsBody.h"

int Monster::count = 0;

Monster::Monster(MonsterType monsterType, seed::View* pView, const Vector2& position)
{
    ++count;

    m_pView = pView;
    SetPosition(position);

    auto pSprite = pView->CreateSprite("crawler.png");
    pSprite->SetScale(Vector2(SPRITE_SCALE));
    pSprite->SetPosition(Vector2(3.f, -2.f) * SPRITE_SCALE);
    pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(pSprite);

    m_pPhysicBody = m_pView->CreateCirclePhysicsForNode(this, .25f, false);
}

Monster::~Monster()
{
    --count;
}

void Monster::Kill()
{
    // Go gore
}

void Monster::UpdateEntity()
{
    if (m_state == MonsterState::IDLE)
    {
        auto pTile = g_gameView->GetTileAt(GetPosition());
        Seek();
    }

    if (m_state == MonsterState::GO_TO)
    {
        Vector2 dir = m_targetPos - GetPosition();
        if (dir.LengthSquared() < .1f * .1f)
        {
            m_state = MonsterState::IDLE;
            Seek();
        }
    }

    if (m_state == MonsterState::GO_TO)
    {
        Vector2 dir = m_targetPos - GetPosition();
        dir.Normalize();

        if (!m_pPhysicBody->GetB2Body()->IsActive())
        {
            m_pPhysicBody->SetTransform(GetPosition() + dir * m_speed * ODT, 0);
        }
        else
        {
            m_pPhysicBody->SetTransform(GetPosition(), 0);
            m_pPhysicBody->SetLinearVel(dir * m_speed);
        }
    }

    Entity::UpdateEntity();
}

void Monster::Seek()
{
    auto pPlayer = g_gameView->GetClosestPlayer(GetPosition());
    if (pPlayer)
    {
        PathTo(pPlayer->GetPosition());
    }
}

void Monster::PathTo(const Vector2& position)
{
    float cost;
    static std::vector<Vector2> path;
    auto ret = g_gameView->findPath(GetPosition(), position, &path, &cost);
    m_path = path;
    if (ret == micropather::MicroPather::START_END_SAME)
    {
        return;
    }
    if (ret == micropather::MicroPather::NO_SOLUTION)
    {
        return;
    }
    if (path.size() < 2) return;
    m_targetPos = path[1];
    auto pTile = g_gameView->GetTileAt(m_targetPos);
    if (pTile)
    {
        if (pTile->isOccupied)
        {
            m_pPhysicBody->GetB2Body()->SetActive(false);
            m_nextUnghost = false;
        }
        else
        {
            if (!m_pPhysicBody->GetB2Body()->IsActive() && m_nextUnghost)
            {
                m_pPhysicBody->GetB2Body()->SetActive(true);
            }
            m_nextUnghost = true;
        }
    }
    m_state = MonsterState::GO_TO;
}

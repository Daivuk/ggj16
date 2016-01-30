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


    m_damageBlood = m_pView->CreateSpriteWithSpriteAnim("fxAnims.spriteanim", "blood");
    m_damageBlood->SetVisible(false);
    Attach(m_damageBlood);

    m_damageSound = m_pView->CreateSoundEmitter("RitualCues_Enemy_Hit.cue");
    m_damageSound->SetPositionBasedBalance(false);
    m_damageSound->SetPositionBasedVolume(false);
    Attach(m_damageSound);
}

Monster::~Monster()
{
    --count;
}

void Monster::Kill()
{
    // Go gore
}

void Monster::AfterDamagePush(const Vector2& in_direction)
{
    m_damageSound->Play();

    m_damageBlood->SetVisible(true);
    m_damageBlood->SetSpriteAnim("");
    m_damageBlood->SetSpriteAnim("blood");
    m_damageBlood->SetScale(Vector2(.1f, .1f));
    m_damageBlood->SetFilter(onut::SpriteBatch::eFiltering::Nearest);


    if (m_state != MonsterState::AFTER_DAMAGE_PUSH)
    {
        m_previousState = m_state;
    }
    m_state = MonsterState::AFTER_DAMAGE_PUSH;

    const float pushForce = 20.f;
    m_velPushAnim.start(in_direction * pushForce, Vector2(0, 0), .2f);
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

    if (m_state == MonsterState::AFTER_DAMAGE_PUSH)
    {
        m_pPhysicBody->SetLinearVel(m_velPushAnim);
        if (!m_velPushAnim.isPlaying())
        {
            // we're done
            m_state = m_previousState;
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

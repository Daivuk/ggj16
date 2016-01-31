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

    m_sprite = pView->CreateSpriteWithSpriteAnim("crawler.spriteanim", "idle_down");
    m_sprite->SetScale(Vector2(SPRITE_SCALE));
    m_sprite->SetPosition(Vector2(3.f, -2.f) * SPRITE_SCALE);
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_sprite);

    m_pPhysicBody = m_pView->CreateCirclePhysicsForNode(this, .25f, false);


    m_damageBlood = m_pView->CreateSpriteWithSpriteAnim("fxAnims.spriteanim", "blood");
    m_damageBlood->SetVisible(false);
    Attach(m_damageBlood);

    m_damageSound = m_pView->CreateSoundEmitter("RitualCues_Enemy_Hit.cue");
    m_damageSound->SetPositionBasedBalance(false);
    m_damageSound->SetPositionBasedVolume(false);
    Attach(m_damageSound);

    m_deathSound = m_pView->CreateSoundEmitter("RitualCues_Enemy_Die.cue");
    m_deathSound->SetPositionBasedBalance(false);
    m_deathSound->SetPositionBasedVolume(false);
    Attach(m_deathSound);
}

Monster::~Monster()
{
    --count;
}

void Monster::Kill()
{
    // Go gore
}

void Monster::OnDeath()
{
    m_deathSound->Play();
    m_sprite->GetScaleAnim().startFromCurrent(m_sprite->GetScale() * 2.f, .3f);
    m_sprite->GetColorAnim().startFromCurrent(Color(1, 1, 1, 0), .3f);
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

        UpdateSpriteAnim(dir);
    }

    if (m_state == MonsterState::AFTER_DAMAGE_PUSH)
    {
        m_pPhysicBody->SetLinearVel(m_velPushAnim);
        if (!m_velPushAnim.isPlaying())
        {
            // we're done
            if (m_health <= 0)
            {
                m_state = MonsterState::DEAD;
                OnDeath();
            }
            else
            {
                m_state = m_previousState;
            }
        }
    }

    if (m_state == MonsterState::DEAD)
    {
        if (!m_sprite->GetScaleAnim().isPlaying())
        {
            // this one is done
            g_gameView->KillEntity(this);
            return;
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

void Monster::UpdateSpriteAnim(const Vector2& dir)
{
    string newAnim;
    bool flipped = false;
    if (dir.y < -0.70710678118654752440084436210485f)
    {
        // move up!
        newAnim = "run_up";
    }

    if (dir.y > 0.70710678118654752440084436210485f)
    {
        // move down!
        newAnim = "run_down";
    }

    if (dir.x < -0.70710678118654752440084436210485f)
    {
        // moving left
        newAnim = "run_down";
        flipped = false;
    }

    if (dir.x > 0.70710678118654752440084436210485f)
    {
        newAnim = "run_down";
        flipped = true;
    }

    if (newAnim.length())
    {
        m_sprite->SetSpriteAnim(newAnim);
        m_sprite->SetFlipped(flipped, false);
    }
}

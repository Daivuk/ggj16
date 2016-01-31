#include "Player.h"
#include "Globals.h"
#include "DanceSequence.h"
#include "PhysicsBody.h"
#include "DancePedestral.h"
#include "GameView.h"
#include "Tile.h"
#include "Monster.h"
#include "Rock.h"
#include "Tree.h"
#include "Drop.h"
#include "Fireplace.h"
#include "Stockpile.h"
#include "Stone.h"
#include "Scarecrow.h"

#define BEHIND_Z_INDEX 10
#define PLAYER_Z_INDEX 20
#define FRONT_Z_INDEX 30
#define HEALTH_Z_INDEX 40

Player::Player()
{
    m_collide = true;
}

Player::~Player()
{
}

void Player::Init(const Vector2& in_position, seed::View* in_container, int in_controllerIndex)
{
    m_controllerIndex = in_controllerIndex;
    m_container = in_container;
    SetPosition(in_position);

    m_slash = m_container->CreateSprite("Staff_Weapon.png");
    m_slash->SetAlign(Vector2(.5f, 1.f));
    m_slash->SetScale(Vector2(SPRITE_SCALE) * .25f);
    m_slash->SetVisible(false);
    m_slash->SetPosition(Vector2(0, -.5f));
    m_slash->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_slash, BEHIND_Z_INDEX);

    m_dottedLine = m_container->CreateSprite("DottedLine.png");
    m_dottedLine->SetScale(Vector2(SPRITE_SCALE));
    m_dottedLine->SetVisible(false);
    m_dottedLine->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_dottedLine);
    auto pA = m_container->CreateSprite("button_a.png");
    pA->SetScale(Vector2(.25f));
    pA->GetPositionAnim().startKeyframed(Vector2(0, -2),
    {
        OAnimWait(Vector2(0, -4), .45f),
        OAnimWait(Vector2(0, -2), .45f)
    }, OLoop);
    pA->SetPosition(Vector2(0, -2));
    pA->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_dottedLine->Attach(pA);

    m_slashSoundEmmiter = m_container->CreateSoundEmitter("RitualCues_Player_Attack.cue");
    m_slashSoundEmmiter->SetPositionBasedVolume(false);
    m_slashSoundEmmiter->SetPositionBasedBalance(true);
    Attach(m_slashSoundEmmiter);

    m_sprite = m_container->CreateSpriteWithSpriteAnim("guruAnims.spriteanim", "idle_down" + std::to_string(m_controllerIndex));
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_sprite->SetScale(Vector2(SPRITE_SCALE * .65f));
    Attach(m_sprite, PLAYER_Z_INDEX);

    m_damageBlood = m_container->CreateSpriteWithSpriteAnim("fxAnims.spriteanim", "blood");
    m_damageBlood->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_damageBlood->SetVisible(false);
    Attach(m_damageBlood);

    m_damageSound = m_container->CreateSoundEmitter("RitualCues_Player_Hit.cue");
    m_damageSound->SetPositionBasedBalance(false);
    m_damageSound->SetPositionBasedVolume(false);
    Attach(m_damageSound);

    m_deathSound = m_container->CreateSoundEmitter("RitualCues_Player_Die.cue");
    m_deathSound->SetPositionBasedBalance(false);
    m_deathSound->SetPositionBasedVolume(false);
    Attach(m_deathSound);

    m_idleAnim = "idle_down" + std::to_string(m_controllerIndex);

    m_physicsBody = in_container->CreateCirclePhysicsForNode(this, .25f, false);

    // health bar
    m_healthGauge = in_container->CreateSprite("healthGauge.png");
    m_healthGauge->SetPosition(Vector2(0,.5f));
    m_healthGauge->SetScale(Vector2(SPRITE_SCALE,SPRITE_SCALE * .5f) * .6f);
    m_healthGauge->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_healthGauge, HEALTH_Z_INDEX);
    m_healthFill = in_container->CreateSprite("healthGaugeFill.png");
    m_healthFill->SetAlign(Vector2(0, .5f));
    m_healthFill->SetPosition(Vector2(m_healthFill->GetWidth() / -2.f, 0));
    m_healthFill->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_healthFill->SetColor(Color(0,1,0));
    m_healthGauge->Attach(m_healthFill);

    m_healthGauge->SetVisible(false);
}

void Player::UpdateHealthBar()
{
    if (!IsAlive())
    {
        m_healthGauge->SetVisible(false);
        return;
    }

    if (m_health == 100.f)
    {
        m_healthGauge->SetVisible(false);
        m_healthFill->SetScale(Vector2(1, 1));
        return;
    }

    m_healthGauge->SetVisible(true);
    if (m_lastHealthValue != m_health)
    {
        // gotta anim it
        m_healthAnim.start(m_lastHealthValue, m_health, .2f, OEaseOut);
        m_lastHealthValue = m_health;
    }

    float percent = m_healthAnim.get() / 100.f;
    m_healthFill->SetScale(Vector2(percent, 1));
    m_healthFill->SetColor(Color(1 - percent, percent, 0));
}

void Player::UpdateEntity()
{
    UpdateHealthBar();
    UpdateInputs();
    UpdateVel();
    UpdatePedestralSnap();
    Entity::UpdateEntity();
    UpdateStoneIndicator();

    if (m_playerState == PlayerState::CARYING_STUFF && m_pCarryOn)
    {
        switch (m_currentDirection)
        {
            case ePlayerDirection::LEFT:
                m_pCarryOn->SetPosition(Vector2(-.5f, -.25f));
                if (m_pCarryOn->GetZindex() < 0)
                {
                    Detach(m_pCarryOn);
                    Attach(m_pCarryOn, 40);
                }
                break;
            case ePlayerDirection::RIGHT:
                m_pCarryOn->SetPosition(Vector2(.5f, -.25f));
                if (m_pCarryOn->GetZindex() < 0)
                {
                    Detach(m_pCarryOn);
                    Attach(m_pCarryOn, 40);
                }
                break;
            case ePlayerDirection::UP:
                m_pCarryOn->SetPosition(Vector2(0.f, -1.0f));
                if (m_pCarryOn->GetZindex() > 0)
                {
                    Detach(m_pCarryOn);
                    Attach(m_pCarryOn, -2);
                }
                break;
            case ePlayerDirection::DOWN:
                m_pCarryOn->SetPosition(Vector2(0.f, 0.f));
                if (m_pCarryOn->GetZindex() < 0)
                {
                    Detach(m_pCarryOn);
                    Attach(m_pCarryOn, 40);
                }
                break;
        }
    }

    // Hax0rz
    if (m_physicsBody)
    {
        if (m_physicsBody->GetPosition().x < 1) 
            m_physicsBody->SetTransform(Vector2(1, GetPosition().y), 0.f);
        if (m_physicsBody->GetPosition().x >(float)g_gameView->GetMapWidth() - 1) 
            m_physicsBody->SetTransform(Vector2((float)g_gameView->GetMapWidth() - 1, GetPosition().y), 0);
        if (m_physicsBody->GetPosition().y < 1) 
            m_physicsBody->SetTransform(Vector2(GetPosition().x, 1), 0.f);
        if (m_physicsBody->GetPosition().y >(float)g_gameView->GetMapHeight() - 1) 
            m_physicsBody->SetTransform(Vector2(GetPosition().x, (float)g_gameView->GetMapHeight() - 1), 0);
    }
}

void Player::UpdateStoneIndicator()
{
    if (m_playerState == PlayerState::CARYING_STUFF)
    {
        if (m_pCarryOn)
        {
            auto pStone = dynamic_cast<Stone*>(m_pCarryOn);
            auto pScarecrow = dynamic_cast<Scarecrow*>(m_pCarryOn);
            if (pStone || pScarecrow)
            {
                auto myPos = GetPosition();
                auto snappedPosition = myPos;
                snappedPosition.x = std::floor(snappedPosition.x) + .5f;
                snappedPosition.y = std::floor(snappedPosition.y) + .5f;
                m_dottedLine->SetVisible(true);
                switch (m_currentDirection)
                {
                    case ePlayerDirection::LEFT:
                        m_dottedLine->SetPosition(Vector2(snappedPosition.x - myPos.x - 1, snappedPosition.y - myPos.y));
                        break;
                    case ePlayerDirection::RIGHT:
                        m_dottedLine->SetPosition(Vector2(snappedPosition.x + 1 - myPos.x, snappedPosition.y - myPos.y));
                        break;
                    case ePlayerDirection::UP:
                        m_dottedLine->SetPosition(Vector2(snappedPosition.x - myPos.x, snappedPosition.y - 1 - myPos.y));
                        break;
                    case ePlayerDirection::DOWN:
                        m_dottedLine->SetPosition(Vector2(snappedPosition.x - myPos.x, snappedPosition.y + 1 - myPos.y));
                        break;
                }
            }
        }
    }
}

void Player::OnRender()
{
}

void Player::UpdateVel()
{
    if (!IsAlive())
    {
        m_physicsBody->SetTransform(GetPosition(), 0);
        m_physicsBody->SetLinearVel(Vector2(0,0));
        m_physicsBody->GetB2Body()->SetActive(false);
        m_sprite->GetPositionAnim().stop(true);
        return;
    }

    if (m_playerState == PlayerState::RECEIVING_DAMAGE)
        return;

    const float maxSpeed = 5.f;
    if (m_thumb.LengthSquared() == 0 || m_playerState == PlayerState::PEDESTRAL)
    {
        // not pressing anything or on a dance pedestral, slowly decellerate
        m_vel = Vector2(0,0);
        m_sprite->GetPositionAnim().stop(true);
    }
    else
    {
        // apply thumb pressure to velocity
        m_vel = m_thumb * maxSpeed;
        if (!m_sprite->GetPositionAnim().isPlaying())
        {
            m_sprite->GetPositionAnim().startKeyframed(
                Vector2::Zero,
                {
                    {Vector2(0, -2) * SPRITE_SCALE, .1f, OEaseOut},
                    {Vector2::Zero, .1f, OEaseIn},
                }, OLoop);
        }
    }
    if (!m_isDancing) UpdateSpriteAnim();
    //Vector2 newPos = GetPosition();
    //newPos += m_vel * ODT;
    
    m_physicsBody->SetTransform(GetPosition(), 0);
    m_physicsBody->SetLinearVel(m_vel);

    // check if we are in the fireplace
    Fireplace* fire = g_gameView->GetFireplace();
    const float fireplaceRadius = .5f;
    if ((fire->GetPosition() - GetPosition()).LengthSquared() < fireplaceRadius * fireplaceRadius)
    {
        // loose health
        m_health -= 1;
        if (m_health < 0) m_health = 0;

        
        // this player dies
        if (m_health <= 0)
        {
            m_physicsBody->SetTransform(fire->GetPosition(), 0);
            OnSacrifice();
        }
        else
        {
            if (!m_healthAnim.isPlaying())
            {
                m_damageSound->Play();
            }
        }
    }
}

void Player::OnSacrifice()
{
    m_health = 0;
    m_playerState = PlayerState::DEAD;
    OnDeath();
    g_gameView->OnPlayerSacrifice(this);
    m_sprite->SetSpriteAnim("idle_down" + std::to_string(m_controllerIndex));
    m_physicsBody->SetTransform(GetPosition(), 0);
}

void Player::UpdateSpriteAnim()
{
    if (!m_currentDancePedestral)
    {
        float horVel = std::abs(m_vel.x);
        float vertVel = std::abs(m_vel.y);

        string newAnim;
        bool flipped = false;

        if (horVel > 0 || vertVel > 0)
        {
            bool goingHorizontal = horVel > vertVel;
            

            if (goingHorizontal)
            {
                if (m_vel.x < 0)
                {
                    newAnim = "run_side";
                    m_idleAnim = "idle_side" + std::to_string(m_controllerIndex);
                    m_currentDirection = ePlayerDirection::LEFT;
                }
                else
                {
                    newAnim = "run_side";
                    m_idleAnim = "idle_side" + std::to_string(m_controllerIndex);
                    flipped = true;
                    m_currentDirection = ePlayerDirection::RIGHT;
                }
            }
            else
            {
                if (m_vel.y < 0)
                {
                    // move up!
                    newAnim = "run_up";
                    m_idleAnim = "idle_up" + std::to_string(m_controllerIndex);
                    m_currentDirection = ePlayerDirection::UP;
                }
                else
                {
                    // move down!
                    newAnim = "run_down";
                    m_idleAnim = "idle_down" + std::to_string(m_controllerIndex);
                    m_currentDirection = ePlayerDirection::DOWN;
                }
            }
        }

        if (newAnim.length())
        {
            m_sprite->SetSpriteAnim(newAnim + std::to_string(m_controllerIndex));
            m_sprite->SetFlipped(flipped, false);
        }
        else
        {
            m_sprite->SetSpriteAnim(m_idleAnim);
        }
    }

}

void Player::Attack()
{
    m_playerState = PlayerState::ATTACKING;

    // attack
    m_slashSoundEmmiter->Play();
    m_slash->SetVisible(true);

    Detach(m_slash);

    m_slash->SetFlippedH(false);
    m_slash->SetFlippedV(false);

    Vector2 attackOffset;

    if (m_currentDirection == ePlayerDirection::RIGHT)
    {
        m_slash->SetFlippedH(true);
        m_slash->GetAngleAnim().start(0, 140, .1f);
        Attach(m_slash, BEHIND_Z_INDEX);
        attackOffset = Vector2(1, 0);
    }
    else if (m_currentDirection == ePlayerDirection::LEFT)
    {
        m_slash->GetAngleAnim().start(0, -140, .1f);
        Attach(m_slash, BEHIND_Z_INDEX);
        attackOffset = Vector2(-1, 0);
    }
    else if (m_currentDirection == ePlayerDirection::DOWN)
    {
        m_slash->SetFlippedH(true);
        m_slash->GetAngleAnim().start(90, 270, .1f);
        Attach(m_slash, FRONT_Z_INDEX);
        attackOffset = Vector2(0, 1);
    }
    else if (m_currentDirection == ePlayerDirection::UP)
    {
        m_slash->SetFlippedH(true);
        m_slash->GetAngleAnim().start(-90, 90, .1f);
        Attach(m_slash, BEHIND_Z_INDEX);
        attackOffset = Vector2(0, -1);
    }

    m_stateTimer.start(.2f, [this]{m_playerState = PlayerState::IDLE; });

    // check if we hit enemies
    vector<Entity*> enemies = g_gameView->GetEntitiesInRadius(m_position + attackOffset * .5f, 2.0f);
    int nbMonster = 0;
    bool bEnemyHit = false;
    for (Entity* e : enemies)
    {
        Monster* monster = dynamic_cast<Monster*>(e);
        if (monster)
        {
            bEnemyHit = true;
            monster->InflictDamage(50.f);
            Vector2 dir = monster->GetPosition() - m_position;
            dir.Normalize();
            monster->AfterDamagePush(dir);
        }
    }

    if (!bEnemyHit)
    {
        // Get the closest
        Entity* pClosest = nullptr;
        float distClosest = 10000.f;
        for (Entity* e : enemies)
        {
            float dist = Vector2::DistanceSquared(e->GetPosition(), GetPosition());
            if (dist < distClosest)
            {
                Rock* rock = dynamic_cast<Rock*>(e);
                if (rock)
                {
                    distClosest = dist;
                    pClosest = rock;
                    continue;
                }
                Tree* tree = dynamic_cast<Tree*>(e);
                if (tree)
                {
                    distClosest = dist;
                    pClosest = tree;
                    continue;
                }
            }
        }
        if (pClosest)
        {
            if (pClosest->InflictDamage(1))
            {
                Rock* rock = dynamic_cast<Rock*>(pClosest);
                Tree* tree = dynamic_cast<Tree*>(pClosest);
                if (rock)
                {
                    m_stateTimer.stop();
                    m_playerState = PlayerState::CARYING_STUFF;
                    m_pCarryOn = new Drop(m_container, DropType::Rock);
                    Attach(m_pCarryOn);
                    OPlaySound("RitualSFX_Stone_Collect.wav");
                }
                else if (tree)
                {
                    m_stateTimer.stop();
                    m_playerState = PlayerState::CARYING_STUFF;
                    m_pCarryOn = new Drop(m_container, DropType::Wood);
                    Attach(m_pCarryOn);
                    OPlaySound("RitualSFX_Wood_Collect.wav");
                }
                auto pTile = g_gameView->GetTileAt(pClosest->GetPosition());
                if (pTile) pTile->isOccupied = false;
                g_gameView->KillEntity(pClosest);
            }
        }
    }
}


float Clamp(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}

void Player::OnDeath()
{
    m_vel = Vector2(0, 0);
    m_physicsBody->SetLinearVel(Vector2(0, 0));
    m_physicsBody->SetAngularVel(0);
    m_physicsBody->SetTransform(GetPosition(), 0);

    m_healthGauge->SetVisible(false);
    m_deathSound->Play();
    m_sprite->GetColorAnim().start(Color(1, 1, 1, 1), Color(0, 0, 0, 1), .5f);
}

void Player::UpdateInputs()
{
    if (!IsAlive())
        return;

    if (m_playerState == PlayerState::RECEIVING_DAMAGE)
    {
        m_physicsBody->SetLinearVel(m_velPushAnim);
        if (!m_velPushAnim.isPlaying())
        {
            // we're done
            if (m_health <= 0)
            {
                m_playerState = PlayerState::DEAD;
                OnDeath();
            }
            else
            {
                m_playerState = PlayerState::IDLE;
            }
        }
        else
        {
            m_vel = Vector2(0, 0);
        }
        return;
    }


    m_thumb = OLThumb(m_controllerIndex);
    m_thumb.x = Clamp(m_thumb.x, -1.f, 1.f);
    m_thumb.y = Clamp(m_thumb.y, -1.f, 1.f);

    if (m_thumb.LengthSquared() > 1.f)
    {
        m_thumb.Normalize();
    }

    if (m_playerState == PlayerState::PEDESTRAL)
    {
        DanceMoveButtonVect& buttons = DanceSequence::GetPossibleButtons();
        for (size_t i = 0, size = buttons.size(); i < size; ++i)
        {
            if (OGamePadJustPressed(buttons[i], m_controllerIndex))
            {
                m_inputSequence.push_back(buttons[i]);
            }
        }
    }
    else
    {
        if (!m_slash->GetAngleAnim().isPlaying())
        {
            m_slash->SetVisible(false);
        }

        if (m_playerState == PlayerState::IDLE)
        {
            if (OGamePadJustPressed(OABtn, m_controllerIndex))
            {
                Attack();
            }
        }
        else if (m_pCarryOn)
        {
            auto pStone = dynamic_cast<Stone*>(m_pCarryOn);
            auto pScarecrow = dynamic_cast<Scarecrow*>(m_pCarryOn);
            if (pStone || pScarecrow)
            {
                if (OGamePadJustPressed(OABtn, m_controllerIndex))
                {
                    DropCarryOn();
                }
            }
            else
            {
                if (OGamePadJustPressed(OBBtn, m_controllerIndex))
                {
                    DropCarryOn();
                }
            }
        }
    }
    if (m_playerState == PlayerState::IDLE && g_gameView->GetStockpile()->IsAround(this))
    {
        Entity* pBoughtEntity = nullptr;
        if (OGamePadJustPressed(OYBtn, m_controllerIndex))
        {
            pBoughtEntity = g_gameView->Buy(StoreItemType::Scarecrow);
        }
        else if (OGamePadJustPressed(OXBtn, m_controllerIndex))
        {
            pBoughtEntity = g_gameView->Buy(StoreItemType::Stone);
        }
        if (pBoughtEntity)
        {
            m_pCarryOn = pBoughtEntity;
            m_playerState = PlayerState::CARYING_STUFF;
            Attach(m_pCarryOn);
        }
    }
}

DropType Player::GetDropType() const
{
    if (!m_pCarryOn) return DropType::INVALID;
    if (m_playerState != PlayerState::CARYING_STUFF) return DropType::INVALID;
    auto pDrop = dynamic_cast<Drop*>(m_pCarryOn);
    if (!pDrop) return DropType::INVALID;
    return pDrop->type;
}

void Player::UpdatePedestralSnap()
{
    if (m_playerState != PlayerState::PEDESTRAL
        && g_gameView->GetTimeOfDay() == TimeOfDay::Night
        && m_thumb.LengthSquared() == 0.f
        && m_playerState != PlayerState::RECEIVING_DAMAGE)
    {
        // check if we should snap to a apedestral
        Tile* tile = g_gameView->GetTileAt(m_position);
        if (tile)
        {
            for (auto entity = tile->pEntities->Head(); entity; entity = tile->pEntities->Next(entity))
            {
                DancePedestral* pedestral = dynamic_cast<DancePedestral*>(entity);
                if (pedestral && !pedestral->m_isOccupied)
                {
                    // we can occupy it
                    OnPedestralLockedIn(pedestral);
                }
            }
        }
    }
    else if (m_playerState == PlayerState::PEDESTRAL && g_gameView->GetTimeOfDay() == TimeOfDay::Night && m_thumb.LengthSquared() >= .9f)
    {
        // player want to leave the dance
        OnPedestralLockCancel();
    }
}

void Player::OnPedestralLockedIn(DancePedestral* in_pedestral)
{
    in_pedestral->StartActivatedFX();
    m_stateTimer.stop();
    DropCarryOn();
    m_playerState = PlayerState::PEDESTRAL;
    m_container->GetPhysicsForNode(this)->SetTransform(in_pedestral->GetPosition(), 0);
    m_sprite->SetSpriteAnim("idle_down" + std::to_string(m_controllerIndex));
    in_pedestral->m_isOccupied = true;
    m_currentDancePedestral = in_pedestral;
    m_slash->SetVisible(false);
}

void Player::DropCarryOn()
{
    if (!m_pCarryOn) return;

    auto pStone = dynamic_cast<Stone*>(m_pCarryOn);
    auto pScarecrow = dynamic_cast<Scarecrow*>(m_pCarryOn);
    if (pStone || pScarecrow)
    {
        auto myPos = GetPosition();
        auto snappedPosition = myPos;
        snappedPosition.x = std::floor(snappedPosition.x) + .5f;
        snappedPosition.y = std::floor(snappedPosition.y) + .5f;
        m_dottedLine->SetVisible(true);
        switch (m_currentDirection)
        {
            case ePlayerDirection::LEFT:
                snappedPosition.x -= 1;
                break;
            case ePlayerDirection::RIGHT:
                snappedPosition.x += 1;
                break;
            case ePlayerDirection::UP:
                snappedPosition.y -= 1;
                break;
            case ePlayerDirection::DOWN:
                snappedPosition.y += 1;
                break;
        }

        auto pTile = g_gameView->GetTileAt(snappedPosition);
        if (pTile)
        {
            if (pTile->isOccupied) return; // Can't place here!
            if (pStone)
            {
                pTile->isOccupied = true;
            }
        }

        m_playerState = PlayerState::IDLE;
        OPlaySound("RitualSFX_Stone_Place.wav");
        m_dottedLine->SetVisible(false);

        Detach(m_pCarryOn);

        if (pStone) pStone->Place(snappedPosition);
        if (pScarecrow) pScarecrow->Place(snappedPosition);

        //m_container->DeleteNode(pStone);
        m_pCarryOn = nullptr;
    }
    else
    {
        Drop* pDrop = (Drop*)(m_pCarryOn);
        if (pDrop->type == DropType::Rock)
        {
            OPlaySoundCue("RitualCues_Stone_Mine.cue");
        }
        if (pDrop->type == DropType::Wood)
        {
            OPlaySoundCue("RitualCues_Wood_Chop.cue");
        }

        m_pCarryOn->SetPosition(GetPosition());
        Detach(m_pCarryOn);
        g_gameView->AddEntity(m_pCarryOn);
        m_pCarryOn = nullptr;
        m_playerState = PlayerState::IDLE;
    }
}

DropType Player::GiveCarryOn()
{
    DropType dropType = ((Drop*)m_pCarryOn)->type;
    m_container->DeleteNode(m_pCarryOn);
    m_pCarryOn = nullptr;
    m_playerState = PlayerState::IDLE;
    return dropType;
}

void Player::OnPedestralLockCancel()
{
    m_isDancing = false;
    if (m_currentDancePedestral)
    {
        m_playerState = PlayerState::IDLE;
        m_currentDancePedestral->m_isOccupied = false;
        m_currentDancePedestral->StartEnabledFX();
        m_currentDancePedestral = nullptr;
    }
}

void Player::OnDanceSequenceSuccess()
{
    if (!m_danceMoveNailed)
    {
        m_danceMoveNailed = m_container->CreateSprite("danceOk.png");
        m_danceMoveNailed->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
        m_sprite->Attach(m_danceMoveNailed);

        string cueName = "RitualCues_Player" + to_string(m_controllerIndex + 1) + "_Drums.cue";
        m_drumSoundEmmiter = m_container->CreateSoundEmitter(cueName);
        m_drumSoundEmmiter->SetPositionBasedVolume(false);
        m_drumSoundEmmiter->SetPositionBasedBalance(false);
        m_sprite->Attach(m_drumSoundEmmiter);
    }

    OPlaySoundCue("RitualCues_ButtonPress.cue");

    m_drumSoundEmmiter->Play();

    m_sprite->SetSpriteAnim("dance" + ::to_string(onut::randi() % 4) + std::to_string(m_controllerIndex));

    m_danceMoveNailed->GetPositionAnim().start(Vector2(0, 0), Vector2(0, -20.f), .2f, OEaseBoth);
    m_danceMoveNailed->GetScaleAnim().startKeyframed(
        Vector2(0,0),
        {
            { Vector2(SPRITE_SCALE, SPRITE_SCALE) * 50.f, .1f, OEaseIn },
            { Vector2(SPRITE_SCALE, SPRITE_SCALE) * 10.f, .1f, OEaseOut },
        });

    m_danceMoveNailed->GetColorAnim().startKeyframed(
        Color(1.f, 1.f, 1.f, 0.f),
        {
            { Color(1.f, 1.f, 1.f, 1.f), .1f },
            OAnimWait(Color(1.f, 1.f, 1.f, 1.f), .3f),
            { Color(1.f, 1.f, 1.f, 0.f), .2f, OLinear },
        });

    // increase health
    const float healthIncreaseForSuccessfulDanceMove = 5;
    m_health += healthIncreaseForSuccessfulDanceMove;
    if (m_health > 100.f) m_health = 100.f;
}

void Player::ResetInputSequence()
{
    m_inputSequence.clear();
}

void Player::AfterDamagePush(const Vector2& in_direction)
{
    m_damageSound->Play();

    m_damageBlood->SetVisible(true);
    m_damageBlood->SetSpriteAnim("");
    m_damageBlood->SetSpriteAnim("blood");
    m_damageBlood->SetScale(Vector2(.1f, .1f));
    m_damageBlood->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    if (m_playerState != PlayerState::RECEIVING_DAMAGE)
    {
        m_lastState = m_playerState;
    }
    OnPedestralLockCancel();
    m_playerState = PlayerState::RECEIVING_DAMAGE;

    const float pushForce = 20.f;
    m_velPushAnim.start(in_direction * pushForce, Vector2(0, 0), .2f);
}

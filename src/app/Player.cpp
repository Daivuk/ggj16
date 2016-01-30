#include "Player.h"
#include "Globals.h"
#include "DanceSequence.h"
#include "PhysicsBody.h"
#include "DancePedestral.h"
#include "GameView.h"
#include "Tile.h"

#define BEHIND_Z_INDEX 10
#define PLAYER_Z_INDEX 20
#define FRONT_Z_INDEX 30

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

    m_slash = m_container->CreateSprite("slash.png");
    m_slash->SetAlign(Vector2(.5f, 1.f));
    m_slash->SetScale(Vector2(SPRITE_SCALE) * .15f);
    m_slash->SetVisible(false);
    m_slash->SetPosition(Vector2(0, -.5f));
    Attach(m_slash, BEHIND_Z_INDEX);

    m_slashSoundEmmiter = m_container->CreateSoundEmitter("RitualCues_Player_Attack.cue");
    m_slashSoundEmmiter->SetPositionBasedVolume(false);
    m_slashSoundEmmiter->SetPositionBasedBalance(true);
    Attach(m_slashSoundEmmiter);

    m_sprite = m_container->CreateSpriteWithSpriteAnim("baltAnims.spriteanim", "idle_down");
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_sprite->SetScale(Vector2(SPRITE_SCALE));
    Attach(m_sprite, PLAYER_Z_INDEX);

    

    m_idleAnim = "idle_down";

    in_container->CreateCirclePhysicsForNode(this, .25f, false);
}

void Player::UpdateEntity()
{
    UpdateInputs();
    UpdateVel();
    UpdatePedestralSnap();
    Entity::UpdateEntity();
}

void Player::OnRender()
{

}

void Player::UpdateVel()
{
    const float maxSpeed = 5.f;
    if (m_thumb.LengthSquared() == 0 || m_isOnPedestral)
    {
        // not pressing anything or on a dance pedestral, slowly decellerate
        m_vel = Vector2(0,0);
    }
    else
    {
        // apply thumb pressure to velocity
        m_vel = m_thumb * maxSpeed;
    }
    UpdateSpriteAnim();
    //Vector2 newPos = GetPosition();
    //newPos += m_vel * ODT;
    
    auto pPhysx = m_container->GetPhysicsForNode(this);
    pPhysx->SetTransform(GetPosition(), 0);
    pPhysx->SetLinearVel(m_vel);
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
                    m_idleAnim = "idle_side";
                    m_currentDirection = ePlayerDirection::LEFT;
                }
                else
                {
                    newAnim = "run_side";
                    m_idleAnim = "idle_side";
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
                    m_idleAnim = "idle_up";
                    m_currentDirection = ePlayerDirection::UP;
                }
                else
                {
                    // move down!
                    newAnim = "run_down";
                    m_idleAnim = "idle_down";
                    m_currentDirection = ePlayerDirection::DOWN;
                }
            }
        }

        if (newAnim.length())
        {
            m_sprite->SetSpriteAnim(newAnim);
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
    // attack
    m_slashSoundEmmiter->Play();
    m_slash->SetVisible(true);

    Detach(m_slash);

    m_slash->SetFlippedH(false);
    m_slash->SetFlippedV(false);


    if (m_currentDirection == ePlayerDirection::RIGHT)
    {
        m_slash->SetFlippedH(true);
        m_slash->GetAngleAnim().start(0, 140, .1f);
        Attach(m_slash, BEHIND_Z_INDEX);
    }
    else if (m_currentDirection == ePlayerDirection::LEFT)
    {
        m_slash->GetAngleAnim().start(0, -140, .1f);
        Attach(m_slash, BEHIND_Z_INDEX);
    }
    else if (m_currentDirection == ePlayerDirection::DOWN)
    {
        m_slash->SetFlippedH(true);
        m_slash->GetAngleAnim().start(90, 270, .1f);
        Attach(m_slash, FRONT_Z_INDEX);
    }
    else if (m_currentDirection == ePlayerDirection::UP)
    {
        m_slash->SetFlippedH(true);
        m_slash->GetAngleAnim().start(-90, 90, .1f);
        Attach(m_slash, BEHIND_Z_INDEX);
    }
}


float Clamp(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}

void Player::UpdateInputs()
{
    m_thumb = OLThumb(m_controllerIndex);
    m_thumb.x = Clamp(m_thumb.x, -1.f, 1.f);
    m_thumb.y = Clamp(m_thumb.y, -1.f, 1.f);

    if (m_thumb.LengthSquared() > 1.f)
    {
        m_thumb.Normalize();
    }

    if (m_isOnPedestral)
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

        if (OGamePadJustPressed(OABtn, m_controllerIndex))
        {
            Attack();
        }
    }
}

void Player::UpdatePedestralSnap()
{
    if (!m_isOnPedestral && g_gameView->GetTimeOfDay() == TimeOfDay::Night && m_thumb.LengthSquared() == 0.f)
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
    else if (m_isOnPedestral && g_gameView->GetTimeOfDay() == TimeOfDay::Night && m_thumb.LengthSquared() >= .9f)
    {
        // player want to leave the dance
        OnPedestralLockCancel();
    }
}

void Player::OnPedestralLockedIn(DancePedestral* in_pedestral)
{
    m_container->GetPhysicsForNode(this)->SetTransform(in_pedestral->GetPosition(), 0);
    m_sprite->SetSpriteAnim("idle_down");
    m_isOnPedestral = true;
    in_pedestral->m_isOccupied = true;
    m_currentDancePedestral = in_pedestral;
}

void Player::OnPedestralLockCancel()
{
    if (m_currentDancePedestral)
    {
        m_isOnPedestral = false;
        m_currentDancePedestral->m_isOccupied = false;
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

    m_drumSoundEmmiter->Play();

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


}

void Player::ResetInputSequence()
{
    m_inputSequence.clear();
}

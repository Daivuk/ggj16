#include "Player.h"
#include "Globals.h"
#include "DanceSequence.h"
#include "PhysicsBody.h"

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
    m_sprite = m_container->CreateSpriteWithSpriteAnim("baltAnims.spriteanim", "idle_down");
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_sprite->SetScale(Vector2(SPRITE_SCALE));
    Attach(m_sprite);

    m_idleAnim = "idle_down";

    in_container->CreateCirclePhysicsForNode(this, .25f, false);
}

void Player::UpdateEntity()
{
    UpdateInputs();
    UpdateVel();
    Entity::UpdateEntity();
}

void Player::OnRender()
{

}

void Player::UpdateVel()
{
    const float maxSpeed = 5.f;
    if (m_thumb.LengthSquared() == 0)
    {
        // not pressing anything, slowly decellerate
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
    string newAnim;
    bool flipped = false;
    if (m_vel.y < 0)
    {
        // move up!
        newAnim = "run_up";
        m_idleAnim = "idle_up";
    }

    if (m_vel.y > 0)
    {
        // move down!
        newAnim = "run_down";
        m_idleAnim = "idle_down";
    }

    if (m_vel.x < 0)
    {
        // moving left
        newAnim = "run_side";
        m_idleAnim = "idle_side";
    }

    if (m_vel.x > 0)
    {
        newAnim = "run_side";
        m_idleAnim = "idle_side";
        flipped = true;
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

    DanceMoveButtonVect& buttons = DanceSequence::GetPossibleButtons();
    for (size_t i = 0, size = buttons.size(); i < size; ++i)
    {
        if (OGamePadJustPressed(buttons[i], m_controllerIndex))
        {
            m_inputSequence.push_back(buttons[i]);
        }
    }
}

void Player::OnDanceSequenceSuccess()
{
    if (!m_danceMoveNailed)
    {
        m_danceMoveNailed = m_container->CreateSprite("danceOk.png");
        m_sprite->Attach(m_danceMoveNailed);
    }

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

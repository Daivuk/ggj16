#include "Player.h"
#include "Globals.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init(const Vector2& in_position, seed::View* in_container, seed::Node* in_parent, int in_controllerIndex)
{
    m_controllerIndex = in_controllerIndex;
    m_container = in_container;
    m_sprite = m_container->CreateSpriteWithSpriteAnim("baltAnims.spriteanim", "idle_down");
    m_sprite->SetPosition(in_position);
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_sprite->SetScale(Vector2(.05f,.05f));
    in_parent->Attach(m_sprite);

    m_idleAnim = "idle_down";
}

void Player::Update()
{
    UpdateInputs();
    UpdateVel();
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
    Vector2 newPos = m_sprite->GetPosition();
    newPos += m_vel * ODT;
    m_sprite->SetPosition(newPos);
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
    m_thumb = OLThumb(m_controllerIndex - 1);
    m_thumb.x = Clamp(m_thumb.x, -1.f, 1.f);
    m_thumb.y = Clamp(m_thumb.y, -1.f, 1.f);

    if (m_thumb.LengthSquared() > 1.f)
    {
        m_thumb.Normalize();
    }
}


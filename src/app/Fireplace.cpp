#include "Fireplace.h"
#include "Globals.h"
#include "GameView.h"


#define DEFAULT_RADIUS 6.f
#define FIREPLACE_SPRITESCALE Vector2(SPRITE_SCALE)

Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    m_position = position;

    m_sprite = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    Attach(m_sprite);
    m_sprite->SetScale(FIREPLACE_SPRITESCALE);
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    auto radius = DEFAULT_RADIUS;
    m_targetRadius = lightRadius = radius;
    lightColor = Color(1.f, .8f, .25f, 1.f) * 1.5f;
    lightEnabled = true;
}

Fireplace::~Fireplace()
{
}

void Fireplace::Grow()
{
    const float growingFactor = 1.f;
    m_targetRadius += growingFactor;
    lightRadius.startKeyframed(
    lightRadius.get(), // From
    {
        // Keyframes
        { m_targetRadius * 1.5f, .1f, OEaseIn },
        { m_targetRadius, .5f, OEaseOut },
    }
    );
}

void Fireplace::UpdateEntity()
{
    Entity::UpdateEntity();

    if (m_gameover)
        return;

    if (!lightRadius.isPlaying() && g_gameView->GetTimeOfDay() == TimeOfDay::Night)
    {
        const float diminishingFactor = 0.5f;
        
        // slowly diminish the fire
        m_targetRadius -= diminishingFactor * ODT;
        lightRadius = m_targetRadius;

        if (m_targetRadius <= 0)
        {
            m_gameover = true;
            m_targetRadius = 0;
            // we are dead!
            g_gameView->OnGameOver();
        }
    }

    // if we are during the night, slowly reduce the fire's radius
    Vector2 scale = lightRadius.get() / DEFAULT_RADIUS * FIREPLACE_SPRITESCALE;
    m_sprite->SetScale(scale);
}

#include "Fireplace.h"
#include "Globals.h"
#include "GameView.h"


#define MAX_RADIUS 10.f
#define DEFAULT_RADIUS 6.f
#define FIREPLACE_SPRITESCALE Vector2(SPRITE_SCALE)

Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    m_position = position;

    m_sprite = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    Attach(m_sprite);
    m_sprite->SetScale(FIREPLACE_SPRITESCALE);
    m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    auto pFireplaceSprite = pView->CreateSprite("fireplace.png");
    pFireplaceSprite->SetScale(FIREPLACE_SPRITESCALE);
    pFireplaceSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    pFireplaceSprite->SetPosition(Vector2(0, .1f));
    Attach(pFireplaceSprite, -1);

    auto radius = DEFAULT_RADIUS;
    m_targetRadius = lightRadius = radius;
    lightColor = Color(1.f, .8f, .25f, 1.f) * 1.5f;
    lightEnabled = true;

    m_whispers = pView->CreateSoundEmitter("RitualSFX_WhispersInTheDarkness.wav");
    Attach(m_whispers);
}

Fireplace::~Fireplace()
{
}

void Fireplace::Grow()
{
    const float growingFactor = 2.f;
    m_targetRadius += growingFactor;
    if (m_targetRadius > MAX_RADIUS)
    {
        m_targetRadius = MAX_RADIUS;
    }
    lightRadius.startKeyframed(
    lightRadius.get(), // From
    {
        // Keyframes
        { m_targetRadius * 1.5f, .1f, OEaseIn },
        { m_targetRadius, .5f, OEaseOut },
    }
    );
}

void Fireplace::GrowToMax()
{
    while (m_targetRadius < MAX_RADIUS)
    {
        Grow();
    }
}

void Fireplace::UpdateEntity()
{
    Entity::UpdateEntity();

    if (m_gameover)
        return;

    // adjust whispers volume
    if (g_gameView->GetTimeOfDay() == TimeOfDay::Night)
    {
        if (!m_whispers->IsPlaying())
        {
            m_whispers->Play();
            m_whispers->SetVolume(0);
        }
        else if (g_gameView->GetTimeOfDay() == TimeOfDay::Night && !m_whispers->GetVolumeAnim().isPlaying() && m_whispers->IsPlaying())
        {
            // lerp to the target value
            float targetVolume = 1.f - m_targetRadius / MAX_RADIUS;
            float currentVolume = m_whispers->GetVolume();
            currentVolume = currentVolume + (targetVolume - currentVolume) * ODT;
            m_whispers->SetVolume(currentVolume);
        }
    }
    else
    {
        if (m_whispers->GetVolume() > 0 && !m_whispers->GetVolumeAnim().isPlaying())
        {
            m_whispers->GetVolumeAnim().startFromCurrent(0, .5f);
        }
        else if (m_whispers->GetVolume() == 0)
        {
            m_whispers->Stop();
        }
    }
    

    
    if (!lightRadius.isPlaying() && g_gameView->GetTimeOfDay() == TimeOfDay::Night)
    {
        const float diminishingFactor = 0.2f;
        
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

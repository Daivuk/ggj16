#include "Fireplace.h"
#include "Globals.h"
#include "GameView.h"


#define MAX_RADIUS 10.f
#define DEFAULT_RADIUS 6.f
#define FIREPLACE_SPRITESCALE Vector2(SPRITE_SCALE,SPRITE_SCALE)
#define FIREPLACE_SPRITESCALE_FX (Vector2(SPRITE_SCALE,SPRITE_SCALE) * .2f)

// balancing stuff
#define DIMINISHING_FACTOR 0.4f
#define GROWING_FACTOR 2.f


Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    m_position = position;

    //m_sprite = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    //Attach(m_sprite);
    //m_sprite->SetScale(FIREPLACE_SPRITESCALE);
    //m_sprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    auto pBase = pView->CreateSprite("light.png");
    pBase->SetBlend(onut::SpriteBatch::eBlendMode::Add);
    pBase->SetScale(Vector2(2.11f, .86f) * FIREPLACE_SPRITESCALE / 6.f);
    pBase->SetColor(Color(118.f / 255.f, 240.f / 255.f, 21.f / 255.f, 1));
    Attach(pBase);

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
    m_whispers->SetLoops(true);
    Attach(m_whispers);

    m_baseSound = pView->CreateSoundEmitter("RitualSFX_Fire_BaseLoop.wav");
    m_baseSound->SetVolume(0);
    m_baseSound->GetVolumeAnim().start(0, .2f, 3.f);
    m_baseSound->SetLoops(true);
    m_baseSound->Play();
    Attach(m_baseSound);

    m_roarSound = pView->CreateSoundEmitter("RitualSFX_Fire_Roar.wav");
    m_roarSound->SetVolume(.1f);
    m_roarSound->SetLoops(true);
    Attach(m_roarSound);
    

    

    m_fireFX1 = pView->CreateEmitter("FirePitFX1.pex");
    m_fireFX1->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_fireFX1->SetBlend(onut::SpriteBatch::eBlendMode::Add);
    m_fireFX1->SetEmitWorld(false);
    m_fireFX1->SetScale(FIREPLACE_SPRITESCALE_FX);
    Attach(m_fireFX1);

    m_fireFX2 = pView->CreateEmitter("FirePitFX2.pex");
    m_fireFX2->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_fireFX1->SetBlend(onut::SpriteBatch::eBlendMode::Add);
    m_fireFX2->SetEmitWorld(false);
    m_fireFX2->SetScale(FIREPLACE_SPRITESCALE_FX);
    Attach(m_fireFX2);

    m_fireFX1->Start();
    m_fireFX2->Start();
}

Fireplace::~Fireplace()
{
}

void Fireplace::Grow()
{
    float playerCount = (float)g_gameView->GetAlivePlayerCount();
    m_targetRadius += GROWING_FACTOR / playerCount;
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

    // adjust whispers+roar volume
    if (g_gameView->GetTimeOfDay() == TimeOfDay::Night)
    {
        // roar
        if (!m_roarSound->IsPlaying())
        {
            m_roarSound->Play();
            m_roarSound->SetVolume(0);
        }
        else if (g_gameView->GetTimeOfDay() == TimeOfDay::Night && !m_roarSound->GetVolumeAnim().isPlaying() && m_roarSound->IsPlaying())
        {
            // lerp to the target value
            float targetVolume = m_targetRadius / MAX_RADIUS;
            float currentVolume = m_roarSound->GetVolume();
            currentVolume = currentVolume + (targetVolume - currentVolume) * ODT;
            m_roarSound->SetVolume(currentVolume);
        }

        // whispers
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
        //roar
        if (m_roarSound->GetVolume() > 0 && !m_roarSound->GetVolumeAnim().isPlaying())
        {
            m_roarSound->GetVolumeAnim().startFromCurrent(0, .5f);
        }
        else if (m_roarSound->GetVolume() == 0)
        {
            m_roarSound->Stop();
        }


        // whipsers
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
        // slowly diminish the fire
        m_targetRadius -= DIMINISHING_FACTOR * ODT;
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
    Vector2 scale = lightRadius.get() / DEFAULT_RADIUS * FIREPLACE_SPRITESCALE_FX;
    m_fireFX1->SetScale(scale);
    m_fireFX2->SetScale(scale);
}

void Fireplace::OnGameOver()
{
    m_baseSound->GetVolumeAnim().startFromCurrent(0, 3.f);
}

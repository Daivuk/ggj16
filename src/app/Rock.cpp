#include "Rock.h"
#include "View.h"
#include "Sprite.h"
#include "Globals.h"

Rock::Rock(seed::View *pView, const Vector2& in_position)
{
    m_health = 5;
    SetPosition(in_position);

    m_pSprite = pView->CreateSprite("rock.png");
    m_pSprite->SetScale(Vector2(SPRITE_SCALE));
    m_pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_pSprite);

    pView->CreateBoxPhysicsForNode(this, true);
}

bool Rock::InflictDamage(float in_damage)
{
    float strengh = 1.25f;
    float timeScale = .75f;
    m_pSprite->GetPositionAnim().startKeyframed(Vector2::Zero,
    {
        {Vector2(-.1f * strengh, 0), .1f * timeScale, OEaseOut},
        {Vector2(.1f * strengh, 0), .1f * timeScale, OEaseBoth},
        {Vector2(-.05f * strengh, 0), .05f * timeScale, OEaseBoth},
        {Vector2(.05f * strengh, 0), .05f * timeScale, OEaseBoth},
        {Vector2(-.025f * strengh, 0), .025f * timeScale, OEaseBoth},
        {Vector2(.025f * strengh, 0), .025f * timeScale, OEaseBoth},
        {Vector2(-.0125f * strengh, 0), .0125f * timeScale, OEaseBoth},
        {Vector2::Zero, .0125f, OEaseIn},
    });
    OPlaySoundCue("RitualCues_Stone_Mine.cue");
    return Entity::InflictDamage(in_damage);
}

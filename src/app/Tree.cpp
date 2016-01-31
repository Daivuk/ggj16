#include "Tree.h"
#include "View.h"
#include "Sprite.h"
#include "Globals.h"

Tree::Tree(seed::View *pView, const Vector2& in_position)
{
    m_health = 5;
    SetPosition(in_position);

    m_pSprite = pView->CreateSprite("tree" + std::to_string(onut::randi()% 3) + ".png");
    m_pSprite->SetAlign(Vector2(.5f, 0.875f));
    m_pSprite->SetScale(Vector2(SPRITE_SCALE));
    m_pSprite->SetPosition(Vector2(0, .25f) * SPRITE_SCALE);
    m_pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_pSprite);

    pView->CreateBoxPhysicsForNode(this, true);
}

bool Tree::InflictDamage(float in_damage)
{
    float strengh = .5f;
    float timeScale = .75f;
    m_pSprite->GetAngleAnim().startKeyframed(0.f,
    {
        {-30.f * strengh, .1f * timeScale, OEaseOut},
        {30.f * strengh, .1f * timeScale, OEaseBoth},
        {-15.f * strengh, .05f * timeScale, OEaseBoth},
        {15.f * strengh, .05f * timeScale, OEaseBoth},
        {-7.5f * strengh, .025f * timeScale, OEaseBoth},
        {7.5f * strengh, .025f * timeScale, OEaseBoth},
        {-3.75f * strengh, .0125f * timeScale, OEaseBoth},
        {0.f, .0125f, OEaseIn},
    });
    OPlaySoundCue("RitualCues_Wood_Chop.cue");
    return Entity::InflictDamage(in_damage);
}

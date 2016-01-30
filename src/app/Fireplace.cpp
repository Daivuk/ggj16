#include "Fireplace.h"
#include "Globals.h"

Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    m_position = position;

    auto pFireAnim = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    Attach(pFireAnim);
    pFireAnim->SetScale(Vector2(SPRITE_SCALE));
    pFireAnim->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    auto radius = 18.f * SPRITE_SCALE;
    pFireAnim->lightRadius = radius;
    pFireAnim->lightColor = Color(1.f, .8f, .25f, 1.f) * 1.5f;
    pFireAnim->lightEnabled = true;
    pFireAnim->lightRadius.startKeyframed(
        radius, {
            {radius - 1.f * SPRITE_SCALE, .25f, OEaseBoth},
            {radius + 1.f * SPRITE_SCALE, .25f, OEaseBoth},
            {radius, .25f * SPRITE_SCALE, OEaseBoth},
            {radius + .5f * SPRITE_SCALE, .25f, OEaseBoth},
        }, OLoop);
}

Fireplace::~Fireplace()
{
}

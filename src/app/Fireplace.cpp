#include "Fireplace.h"
#include "Globals.h"

Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    m_position = position;

    auto pFireAnim = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    Attach(pFireAnim);
    pFireAnim->SetScale(Vector2(SPRITE_SCALE));
    pFireAnim->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    auto radius = 6.f;
    lightRadius = radius;
    lightColor = Color(1.f, .8f, .25f, 1.f) * 1.5f;
    lightEnabled = true;
}

Fireplace::~Fireplace()
{
}

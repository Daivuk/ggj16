#include "Fireplace.h"
#include "Globals.h"

Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    m_position = position;

    auto pFireAnim = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    pFireAnim->SetScale(Vector2(SPRITE_SCALE));
    Attach(pFireAnim);
}

Fireplace::~Fireplace()
{
}

#include "Fireplace.h"
#include "Globals.h"

Fireplace::Fireplace(seed::View* pView, const Vector2& position)
{
    auto pFireAnim = pView->CreateSpriteWithSpriteAnim("fireplace.spriteanim", "fire1");
    pFireAnim->SetPosition(position);
    pFireAnim->SetScale(Vector2(SPRITE_SCALE));
    pView->AddNode(pFireAnim);
}

Fireplace::~Fireplace()
{
}

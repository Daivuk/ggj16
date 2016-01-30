#include "Tree.h"
#include "View.h"
#include "Sprite.h"
#include "Globals.h"

Tree::Tree(seed::View *pView, const Vector2& in_position)
{
    SetPosition(in_position);

    auto pSprite = pView->CreateSprite("tree.png");
    pSprite->SetScale(Vector2(SPRITE_SCALE));
    pSprite->SetPosition(Vector2(0, -.75f));
    pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(pSprite);

    pView->CreateBoxPhysicsForNode(this, true);
}

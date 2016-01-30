#include "Tree.h"
#include "View.h"
#include "Sprite.h"
#include "Globals.h"

Tree::Tree(seed::View *pView)
{
    auto pSprite = pView->CreateSprite("tree.png");
    pSprite->SetScale(Vector2(SPRITE_SCALE));
    pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(pSprite);
}

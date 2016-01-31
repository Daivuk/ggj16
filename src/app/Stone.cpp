#include "Stone.h"
#include "View.h"
#include "Globals.h"
#include "Sprite.h"

Stone::Stone(seed::View* pView)
{
    m_pSprite = pView->CreateSprite("stone.png");
    m_pSprite->SetScale(Vector2(SPRITE_SCALE));
    m_pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_pSprite);
}

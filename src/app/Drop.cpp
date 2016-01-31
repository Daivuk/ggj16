#include "Drop.h"
#include "Sprite.h"
#include "View.h"
#include "Globals.h"

Drop::Drop(seed::View* pView, DropType in_type)
    : m_pView(pView)
    , type(in_type)
{
    type = in_type;
    seed::Sprite* pSprite = nullptr;
    switch (type)
    {
        case DropType::Wood:
            pSprite = m_pView->CreateSprite("woodCarryH.png");
            break;
        case DropType::Rock:
            pSprite = m_pView->CreateSprite("carryRock.png");
            break;
    }
    pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    pSprite->SetScale(Vector2(SPRITE_SCALE));
    Attach(pSprite);
}

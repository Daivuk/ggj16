#include "Rock.h"
#include "View.h"
#include "Sprite.h"
#include "Globals.h"

Rock::Rock(seed::View *pView, const Vector2& in_position)
{
    m_health = 5;
    SetPosition(in_position);

    auto pSprite = pView->CreateSprite("rock.png");
    pSprite->SetScale(Vector2(SPRITE_SCALE));
    pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(pSprite);

    pView->CreateBoxPhysicsForNode(this, true);
}

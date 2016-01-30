#include "Monster.h"
#include "View.h"
#include "Globals.h"
#include "Sprite.h"

int Monster::count = 0;

Monster::Monster(MonsterType monsterType, seed::View* pView, const Vector2& position)
{
    ++count;

    m_pView = pView;
    SetPosition(position);

    auto pSprite = pView->CreateSprite("crawler.png");
    pSprite->SetScale(Vector2(SPRITE_SCALE));
    pSprite->SetPosition(Vector2(3.f, -2.f) * SPRITE_SCALE);
    pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(pSprite);

    m_pView->CreateCirclePhysicsForNode(this, .25f, false);
}

Monster::~Monster()
{
    --count;
}

#include "Stone.h"
#include "View.h"
#include "Globals.h"
#include "Sprite.h"
#include "GameView.h"

Stone::Stone(seed::View* pView)
    : m_pView(pView)
{
    m_pSprite = pView->CreateSprite("stone" + std::to_string(onut::randi() % 4) + ".png");
    m_pSprite->SetScale(Vector2(SPRITE_SCALE));
    m_pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_pSprite);
}

void Stone::Place(const Vector2& position)
{
    SetPosition(position);
    g_gameView->AddEntity(this);

    m_pView->CreateBoxPhysicsForNode(this, true);
}

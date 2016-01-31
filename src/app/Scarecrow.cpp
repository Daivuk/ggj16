#include "Scarecrow.h"
#include "View.h"
#include "Globals.h"
#include "Sprite.h"
#include "GameView.h"

Scarecrow::Scarecrow(seed::View* pView)
    : m_pView(pView)
{
    m_pSprite = pView->CreateSprite("scarecrow.png");
    m_pSprite->SetScale(Vector2(SPRITE_SCALE));
    m_pSprite->SetAlign(Vector2(.5f, .75f));
    m_pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_pSprite);
}

void Scarecrow::Place(const Vector2& position)
{
    SetPosition(position);
    g_gameView->AddEntity(this);

    m_pView->CreateCirclePhysicsForNode(this, .15f, true);
}

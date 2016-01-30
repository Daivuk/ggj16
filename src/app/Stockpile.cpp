#include "Stockpile.h"
#include "Globals.h"
#include "View.h"
#include "Sprite.h"
#include "GameView.h"

Stockpile::Stockpile(seed::View* pView, int x, int y)
    : m_pView(pView)
{
    SetPosition(Vector2((float)x + 1.f, (float)y + .5f));
    auto pStockpileSprite = m_pView->CreateSprite("stockpile.png");
    pStockpileSprite->SetScale(Vector2(SPRITE_SCALE));
    pStockpileSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    pStockpileSprite->SetAlign(Vector2::Zero);
    pStockpileSprite->SetPosition(Vector2(-2 - 16, -7 - 16 + 8) * SPRITE_SCALE);
    Attach(pStockpileSprite);

    auto pTile = g_gameView->GetTileAt(x, y);
    if (pTile) pTile->isOccupied = true;
    pTile = g_gameView->GetTileAt(x + 1, y);
    if (pTile) pTile->isOccupied = true;

    pView->CreateBoxPhysicsForNode(this, true);
}

Stockpile::~Stockpile()
{
}

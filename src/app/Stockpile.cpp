#include "Stockpile.h"
#include "Globals.h"
#include "View.h"
#include "Sprite.h"
#include "GameView.h"
#include "Player.h"

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

void Stockpile::UpdateEntity()
{
    auto& players = g_gameView->GetPlayers();
    for (auto pPlayer : players)
    {
        if (pPlayer)
        {
            if (pPlayer->HasCarryOn())
            {
                auto& playerPos = pPlayer->GetPosition();
                auto& myPos = GetPosition();
                if (playerPos.x >= myPos.x - 1.5f &&
                    playerPos.y >= myPos.y - 1.f &&
                    playerPos.x <= myPos.x + 1.5f &&
                    playerPos.y >= myPos.y + 1.f)
                {
                    auto dropType = pPlayer->GiveCarryOn();
                }
            }
        }
    }
}

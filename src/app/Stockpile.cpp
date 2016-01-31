#include "Stockpile.h"
#include "Globals.h"
#include "View.h"
#include "Sprite.h"
#include "GameView.h"
#include "Player.h"
#include "SpriteString.h"
#include <sstream>
#include <iomanip>

enum class StoreItemType
{
    Scarecrow,
    Stone
};

struct StoreItem
{
    StoreItemType type;
    int wood;
    int rock;
    std::string ui;
    onut::GamePad::eGamePad buyButton;
};

static const std::vector<StoreItem> store = {
    {StoreItemType::Scarecrow, 3, 0, "scarecrow", OABtn},
    {StoreItemType::Scarecrow, 0, 2, "stone", OXBtn}
};

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

    m_pRockSprite = m_pView->CreateSprite("carryRock.png");
    m_pRockSprite->SetScale(Vector2(.65f));
    m_pRockSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_pRockSprite->SetPosition(Vector2(8, 4));
    pStockpileSprite->Attach(m_pRockSprite);

    m_pRockText = m_pView->CreateSpriteString("font.fnt");
    m_pRockText->SetCaption("00");
    m_pRockText->SetScale(Vector2(.5f));
    m_pRockText->SetColor(Color::Black);
    m_pRockText->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_pRockText->SetPosition(Vector2(16, 4));
    pStockpileSprite->Attach(m_pRockText);

    m_pWoodSprite = m_pView->CreateSprite("woodCarryH.png");
    m_pWoodSprite->SetScale(Vector2(.65f));
    m_pWoodSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_pWoodSprite->SetPosition(Vector2(22, 4));
    pStockpileSprite->Attach(m_pWoodSprite);

    m_pWoodText = m_pView->CreateSpriteString("font.fnt");
    m_pWoodText->SetCaption("00");
    m_pWoodText->SetScale(Vector2(.5f));
    m_pWoodText->SetColor(Color::Black);
    m_pWoodText->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_pWoodText->SetPosition(Vector2(28, 4));
    pStockpileSprite->Attach(m_pWoodText);

    auto pTile = g_gameView->GetTileAt(x, y);
    if (pTile) pTile->isOccupied = true;
    pTile = g_gameView->GetTileAt(x + 1, y);
    if (pTile) pTile->isOccupied = true;

    pView->CreateBoxPhysicsForNode(this, true);

    lightEnabled = true;
    lightRadius = 3.f;
    lightColor = Color::White * .5f;
}

Stockpile::~Stockpile()
{
}

void Stockpile::UpdateEntity()
{
    bool bShowStore = false;
    auto& players = g_gameView->GetPlayers();
    for (auto pPlayer : players)
    {
        if (pPlayer)
        {
            auto& playerPos = pPlayer->GetPosition();
            auto& myPos = GetPosition();
            if (playerPos.x >= myPos.x - 1.5f &&
                playerPos.y >= myPos.y - 1.f &&
                playerPos.x <= myPos.x + 1.5f &&
                playerPos.y <= myPos.y + 1.f)
            {
                bShowStore = true;
                if (pPlayer->HasCarryOn())
                {
                    auto dropType = pPlayer->GiveCarryOn();
                    resources[dropType]++;
                    if (dropType == DropType::Rock)
                    {
                        OPlaySound("RitualSFX_Stone_Collect.wav");
                        m_pRockSprite->GetPositionAnim().startKeyframed(Vector2(8, 4),
                            {OAnimAppleStyleBounce(Vector2(8, 4), Vector2(8, -2))});
                        std::stringstream ss;
                        ss << std::setw(2) << std::setfill('0') << resources[dropType];
                        m_pRockText->SetCaption(ss.str());
                    }
                    if (dropType == DropType::Wood)
                    {
                        OPlaySound("RitualSFX_Wood_Collect.wav");
                        m_pWoodSprite->GetPositionAnim().startKeyframed(Vector2(22, 4),
                            {OAnimAppleStyleBounce(Vector2(22, 4), Vector2(22, -2))});
                        std::stringstream ss;
                        ss << std::setw(2) << std::setfill('0') << resources[dropType];
                        m_pWoodText->SetCaption(ss.str());
                    }
                }
            }
        }
    }

    if (bShowStore && !m_bSomeoneAround)
    {
        g_gameView->ShowStore();
    }
    if (!bShowStore && m_bSomeoneAround)
    {
        g_gameView->HideStore();
    }
    m_bSomeoneAround = bShowStore;
}

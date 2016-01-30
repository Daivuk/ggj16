#include "GameView.h"
#include "Player.h"
#include "TiledMapNode.h"

const Vector2 g_playerSpawn[] = {
    {14.f, 14.f},
    {18.f, 18.f},
    {14.f, 18.f},
    {18.f, 14.f},
};

GameView::GameView()
{
}

GameView::~GameView()
{
}

void GameView::OnShow()
{
    // spawn players from the lobby data, for now assume one
    CreateTileMap();
    GenerateMap();
    CenterCamera();
    SpawnPlayers();
}

void GameView::OnHide()
{
}

void GameView::OnUpdate()
{
    UpdatePlayers();

    // Update camera based on the players position
    GetRootNode()->SetScale(Vector2(m_zoom));
    GetRootNode()->SetPosition(-m_camera * m_zoom + OScreenf * .5f);
}

void GameView::OnRender()
{
}

void GameView::SpawnPlayers()
{
    Player* player1 = new Player();
    // todo pass in what "skin" used
    player1->Init(Vector2(16,16), this, 1);
    m_players.push_back(player1);

    // todo spawn other players
}

void GameView::UpdatePlayers()
{
    for (Player* p : m_players)
    {
        p->Update();
    }
}

void GameView::CreateTileMap()
{
    auto pTileMapNode = CreateTiledMapNode("maptemplate.tmx");
    AddNode(pTileMapNode);

    m_pTilemap = pTileMapNode->GetTiledMap();
    m_pBackgroundLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("backgrounds");
    m_pTileLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("tiles");

    pTileMapNode->SetScale(Vector2(1.f / m_pTilemap->getTileWidth()));
}

void GameView::CenterCamera()
{
    m_camera = Vector2((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);
}

void GameView::GenerateMap()
{
    Vector2 center((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);

    for (int i = 0; i < 50; ++i)
    {
        auto dist = onut::randf(6.f, (float)m_pTilemap->getWidth() * .5f * 1.42f);
        auto angle = onut::randf(0.f, DirectX::XM_2PI);

        Vector2 dir(std::cos(angle) * dist, std::sin(angle) * dist);

        m_pTilemap->setTileAt(m_pTileLayer, (int)dir.x, (int)dir.y, 2);
    }
}

eTile GameView::GetTileAt(const Vector2& position)
{
    return (eTile)m_pTilemap->getTileAt(m_pTileLayer, (int)position.x, (int)position.y);
}

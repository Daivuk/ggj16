#include "GameView.h"
#include "Player.h"
#include "TiledMapNode.h"

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
    CenterCamera();
    SpawnPlayers();
}

void GameView::OnHide()
{
}

void GameView::OnUpdate()
{
    UpdatePlayers();
    m_zoom += ODT;

    // .. do update of our shitz

    // Update camera based on the players position
    GetRootNode()->SetScale(Vector2(m_zoom));
    auto invZoom = (1.f / m_zoom);
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
    m_pTileLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("tiles");

    pTileMapNode->SetScale(Vector2(1.f / m_pTilemap->getTileWidth()));
}

void GameView::CenterCamera()
{
    m_camera = Vector2((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);
}

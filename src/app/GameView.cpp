#include "GameView.h"
#include "TiledMapNode.h"

GameView::GameView()
{
}

GameView::~GameView()
{
}

void GameView::OnShow()
{
    CreateTileMap();
    CenterCamera();
}

void GameView::OnHide()
{
}

void GameView::OnUpdate()
{
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

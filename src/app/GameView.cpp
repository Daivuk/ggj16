#include "GameView.h"
#include "Player.h"
#include "TiledMapNode.h"
#include "Fireplace.h"
#include "DanceSequence.h"
#include "LightLayer.h"

#define TREE_DENSITY 50
#define ROCK_DENSITY 30

const Vector2 g_playerSpawn[] = {
    {15.f, 15.f},
    {17.f, 17.f},
    {15.f, 17.f},
    {17.f, 15.f},
};

GameView::GameView()
{
}

GameView::~GameView()
{
    if (m_pTiles)
    {
        delete[] m_pTiles;
    }
    if (m_pFireplace)
    {
        delete m_pFireplace;
    }
}

void GameView::OnShow()
{
    // Create the main game node. Map + objects go in there and are affected by light
    pGameLayer = CreateLightLayer();
    pGameLayer->SetAmbient(Color(.10f, .15f, .2f, 1)); // Set that so something cool jason will decide on
    AddNode(pGameLayer);

    // spawn players from the lobby data, for now assume one
    CreateTileMap();
    GenerateMap();
    CreateEntities();
    CenterCamera();
    SpawnPlayers();

    // temp
    StartDanceSequence();
}

void GameView::OnHide()
{
}

void GameView::OnUpdate()
{
    // To test map generation, never do that
    //if (OJustPressed(OINPUT_SPACE)) GenerateMap();

    UpdateDanceSequence();

    // Update camera based on the players position
    GetRootNode()->SetScale(Vector2(m_zoom));
    GetRootNode()->SetPosition(-m_camera * m_zoom + OScreenf * .5f);
}

void GameView::OnRender()
{
}

void GameView::SpawnPlayers()
{
    for (int i = 0; i < 4; ++i)
    {
        if (OGamePad(i)->isConnected())
        {
            Player* player = new Player();
            // todo pass in what "skin" used
            player->Init(g_playerSpawn[i], this, 1);
            m_players.push_back(player);
            AddEntity(player);
        }
    }
}

void GameView::StartDanceSequence()
{
    m_activeDanceSequence = new DanceSequence();
    m_activeDanceSequence->Init(m_difficulty, m_pFireplace, this);
}

void GameView::UpdateDanceSequence()
{
    if (m_activeDanceSequence)
    {
        m_activeDanceSequence->Update();
    }
}

void GameView::CreateTileMap()
{
    auto pTileMapNode = CreateTiledMapNode("maptemplate.tmx");
    pGameLayer->Attach(pTileMapNode);

    m_pTilemap = pTileMapNode->GetTiledMap();
    m_pBackgroundLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("backgrounds");
    m_pTileLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("tiles");

    pTileMapNode->SetScale(Vector2(1.f / m_pTilemap->getTileWidth()));

    m_pTiles = new Tile[m_pTilemap->getWidth() * m_pTilemap->getHeight()];
}

void GameView::CenterCamera()
{
    m_camera = Vector2((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);
}

void GameView::GenerateMap()
{
    Vector2 center((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);

    // Spawn a bunch of rockz
    for (int i = 0; i < ROCK_DENSITY; ++i)
    {
        auto center = onut::rand2f(Vector2::Zero, Vector2((float)m_pTilemap->getWidth(), (float)m_pTilemap->getHeight()));
        int count = onut::randi(2, 6);
        for (int j = 0; j < count; ++j)
        {
            auto pos = center += onut::rand2f(Vector2(-3), Vector2(3));
            //m_pTilemap->setTileAt(m_pTileLayer, (int)pos.x, (int)pos.y, 3);
        }
    }

    // Spawn a bunch of trees
    for (int i = 0; i < TREE_DENSITY; ++i)
    {
        auto center = onut::rand2f(Vector2::Zero, Vector2((float)m_pTilemap->getWidth(), (float)m_pTilemap->getHeight()));
        int count = onut::randi(2, 12);
        for (int j = 0; j < count; ++j)
        {
            auto pos = center += onut::rand2f(Vector2(-3), Vector2(3));
            //m_pTilemap->setTileAt(m_pTileLayer, (int)pos.x, (int)pos.y, 2);
        }
    }

    //// Clear a zone in the middle
    //POINT mapCenter = {m_pTilemap->getWidth() / 2, m_pTilemap->getHeight() / 2};
    //for (int y = mapCenter.y - 3; y <= mapCenter.y + 3; ++y)
    //{
    //    for (int x = mapCenter.x - 3; x <= mapCenter.x + 3; ++x)
    //    {
    //        m_pTilemap->setTileAt(m_pTileLayer, x, y, 1);
    //    }
    //}

    // Fireplace dead in the middle
    //m_pTilemap->setTileAt(m_pTileLayer, mapCenter.x, mapCenter.y, 4);
}

eTile GameView::GetTileIdAt(const Vector2& position) const
{
    return (eTile)m_pTilemap->getTileAt(m_pTileLayer, (int)position.x, (int)position.y);
}

Tile *GameView::GetTileAt(const Vector2& position) const
{
    auto x = (int)position.x;
    auto y = (int)position.y;
    if (x < 0 || y < 0 || x >= m_pTilemap->getWidth() || y >= m_pTilemap->getHeight()) return nullptr;

    return m_pTiles + (y * m_pTilemap->getWidth() + x);
}

Vector2 GameView::GetMapCenter() const
{
    return Vector2((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);
}

void GameView::CreateEntities()
{
    m_pFireplace = new Fireplace(this, GetMapCenter());
    AddEntity(m_pFireplace);
}

void GameView::AddEntity(Entity* pEntity)
{
    pGameLayer->Attach(pEntity);
    m_entities.push_back(m_pFireplace);
    auto pTile = GetTileAt(pEntity->GetPosition());
    if (pTile)
    {
        pTile->RegisterEntity(pEntity);
    }
}

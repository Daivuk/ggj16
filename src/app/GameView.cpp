#include "GameView.h"
#include "Player.h"
#include "TiledMapNode.h"
#include "Fireplace.h"
#include "DanceSequence.h"

#define TREE_DENSITY 50
#define ROCK_DENSITY 30

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
    if (m_pFireplace)
    {
        delete m_pFireplace;
    }
}

void GameView::OnShow()
{
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

    UpdatePlayers();
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
    Player* player1 = new Player();
    // todo pass in what "skin" used
    player1->Init(Vector2(16,16), this, 1);
    m_players.push_back(player1);

    // todo spawn other players
}

void GameView::StartDanceSequence()
{
    m_activeDanceSequence = new DanceSequence();
    m_activeDanceSequence->Init(m_difficulty, m_pFireplace);
}

void GameView::UpdateDanceSequence()
{
    if (m_activeDanceSequence)
    {
        m_activeDanceSequence->Update();
    }
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

    for (int y = 0; y < m_pTilemap->getHeight(); ++y)
    {
        for (int x = 0; x < m_pTilemap->getWidth(); ++x)
        {
            m_pTilemap->setTileAt(m_pTileLayer, x, y, 1);
        }
    }

    // Spawn a bunch of rockz
    for (int i = 0; i < ROCK_DENSITY; ++i)
    {
        auto center = onut::rand2f(Vector2::Zero, Vector2((float)m_pTilemap->getWidth(), (float)m_pTilemap->getHeight()));
        int count = onut::randi(2, 6);
        for (int j = 0; j < count; ++j)
        {
            auto pos = center += onut::rand2f(Vector2(-3), Vector2(3));
            m_pTilemap->setTileAt(m_pTileLayer, (int)pos.x, (int)pos.y, 3);
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
            m_pTilemap->setTileAt(m_pTileLayer, (int)pos.x, (int)pos.y, 2);
        }
    }

    // Clear a zone in the middle
    POINT mapCenter = {m_pTilemap->getWidth() / 2, m_pTilemap->getHeight() / 2};
    for (int y = mapCenter.y - 3; y <= mapCenter.y + 3; ++y)
    {
        for (int x = mapCenter.x - 3; x <= mapCenter.x + 3; ++x)
        {
            m_pTilemap->setTileAt(m_pTileLayer, x, y, 1);
        }
    }

    // Fireplace dead in the middle
    m_pTilemap->setTileAt(m_pTileLayer, mapCenter.x, mapCenter.y, 4);
}

eTile GameView::GetTileAt(const Vector2& position) const
{
    return (eTile)m_pTilemap->getTileAt(m_pTileLayer, (int)position.x, (int)position.y);
}

Vector2 GameView::GetMapCenter() const
{
    return Vector2((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);
}

void GameView::CreateEntities()
{
    m_pFireplace = new Fireplace(this, GetMapCenter());
    AddNode(m_pFireplace);
    m_entities.push_back(m_pFireplace);
}

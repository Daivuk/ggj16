#include "GameView.h"
#include "Player.h"
#include "TiledMapNode.h"
#include "Fireplace.h"
#include "DanceSequence.h"
#include "LightLayer.h"
#include "Tree.h"
#include "Rock.h"

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
    m_pGameLayer = CreateLightLayer();
    m_pGameLayer->SetAmbient(Color(.10f, .15f, .2f, 1)); // Set that so something cool jason will decide on
    AddNode(m_pGameLayer);

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
    UpdateTime();

    // To test map generation, never do that
    //if (OJustPressed(OINPUT_SPACE)) GenerateMap();

    UpdateDanceSequence();

    // Update camera based on the players position
    GetRootNode()->SetScale(Vector2(m_zoom));
    GetRootNode()->SetPosition(-m_camera * m_zoom + OScreenf * .5f);
}

void GameView::UpdateTime()
{
    m_dayTime += ODT;
    if (m_dayTime >= DAY_TOTAL_DURATION)
    {
        m_dayTime -= DAY_TOTAL_DURATION;
        m_day++;
    }

    static const Color dayAmbient(1, 1, 1, 1);
    static const Color nightAmbient(.10f, .15f, .2f, 1);
    static const Color dawnAmbient(1, .75f, 0, 1);
    static const Color duskAmbient(.75f, .35f, .55f, 1);

    switch (GetTimeOfDay())
    {
        case TimeOfDay::Day:
            m_pGameLayer->SetAmbient(dayAmbient);
            break;
        case TimeOfDay::Night:
            m_pGameLayer->SetAmbient(nightAmbient);
            break;
        case TimeOfDay::Dawn:
        {
            const float fullStart = DAWN_START + (DAWN_END - DAWN_START) * .3f;
            const float fullEnd = DAWN_START + (DAWN_END - DAWN_START) * .6f;
            if (m_dayTime >= fullStart && m_dayTime <= fullEnd) m_pGameLayer->SetAmbient(dawnAmbient);
            else if (m_dayTime < fullStart)
            {
                float dawnPercent = (m_dayTime - DAWN_START) / (fullStart - DAWN_START);
                m_pGameLayer->SetAmbient(Color::Lerp(nightAmbient, dawnAmbient, dawnPercent));
            }
            else
            {
                float dawnPercent = (DAWN_END - m_dayTime) / (DAWN_END - fullEnd);
                m_pGameLayer->SetAmbient(Color::Lerp(dayAmbient, dawnAmbient, dawnPercent));
            }
            break;
        }
        case TimeOfDay::Dusk:
        {
            const float fullStart = DUSK_START + (DUSK_END - DUSK_START) * .3f;
            const float fullEnd = DUSK_START + (DUSK_END - DUSK_START) * .6f;
            if (m_dayTime >= fullStart && m_dayTime <= fullEnd) m_pGameLayer->SetAmbient(duskAmbient);
            else if (m_dayTime < fullStart)
            {
                float duskPercent = (m_dayTime - DUSK_START) / (fullStart - DUSK_START);
                m_pGameLayer->SetAmbient(Color::Lerp(dayAmbient, duskAmbient, duskPercent));
            }
            else
            {
                float duskPercent = (DUSK_END - m_dayTime) / (DUSK_END - fullEnd);
                m_pGameLayer->SetAmbient(Color::Lerp(nightAmbient, duskAmbient, duskPercent));
            }
            break;
        }
    }
}

float GameView::GetDayTimeHour() const
{
    return (m_dayTime / DAY_TOTAL_DURATION) * 24.f;
}

TimeOfDay GameView::GetTimeOfDay() const
{
    if (m_dayTime >= DAWN_END && m_dayTime <= DUSK_START) return TimeOfDay::Day;
    if (m_dayTime >= DUSK_START && m_dayTime <= DUSK_END) return TimeOfDay::Dusk;
    if (m_dayTime >= DAWN_START && m_dayTime <= DAWN_END) return TimeOfDay::Dawn;
    return TimeOfDay::Night;
}

void GameView::OnRender()
{
    auto pFont = OGetBMFont("font.fnt");
    pFont->draw("Time: " + std::to_string(GetDayTimeHour()), Vector2::Zero);
    switch (GetTimeOfDay())
    {
        case TimeOfDay::Night:
            pFont->draw("Night", Vector2(0, 20));
            break;
        case TimeOfDay::Dawn:
            pFont->draw("Dawn", Vector2(0, 20));
            break;
        case TimeOfDay::Day:
            pFont->draw("Day", Vector2(0, 20));
            break;
        case TimeOfDay::Dusk:
            pFont->draw("Dusk", Vector2(0, 20));
            break;
    }
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
    m_pGameLayer->Attach(pTileMapNode);

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
            pos.x = std::round(pos.x) + .5f;
            pos.y = std::round(pos.y) + .5f;
            if (pos.x >= center.x - 3 && pos.y >= center.y - 3 &&
                pos.x <= center.x + 3 && pos.y <= center.y + 3) continue;
            auto pRock = new Rock(this);
            pRock->SetPosition(pos);
            AddEntity(pRock);
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
            pos.x = std::round(pos.x) + .5f;
            pos.y = std::round(pos.y) + .5f;
            if (pos.x >= center.x - 3 && pos.y >= center.y - 3 &&
                pos.x <= center.x + 3 && pos.y <= center.y + 3) continue;
            auto pTree = new Tree(this);
            pTree->SetPosition(pos);
            AddEntity(pTree);
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
    m_pGameLayer->Attach(pEntity);
    m_entities.push_back(m_pFireplace);
    auto pTile = GetTileAt(pEntity->GetPosition());
    if (pTile)
    {
        pTile->RegisterEntity(pEntity);
    }
}

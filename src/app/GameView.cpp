#include "GameView.h"
#include "Player.h"
#include "TiledMapNode.h"
#include "Fireplace.h"
#include "DanceSequence.h"
#include "LightLayer.h"
#include "Tree.h"
#include "Rock.h"
#include "MusicEmitter.h"
#include "DancePedestral.h"
#include "Monster.h"

#define TREE_DENSITY 50
#define ROCK_DENSITY 30

const Vector2 g_playerSpawn[] = {
    {15.f, 15.f},
    {17.f, 17.f},
    {15.f, 17.f},
    {17.f, 15.f},
};

GameView* g_gameView = nullptr;

GameView::GameView()
{
    g_gameView = this;
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
    InitPhysics(Vector2::Zero, 1.f);

    // Create the main game node. Map + objects go in there and are affected by light
    m_pGameLayer = CreateLightLayer();
    m_pGameLayer->SetAmbient(Color(.10f, .15f, .2f, 1)); // Set that so something cool jason will decide on
    AddNode(m_pGameLayer);

    // spawn players from the lobby data, for now assume one
    CreateMusic();
    CreateTileMap();
    GenerateMap();
    CreateEntities();
    CenterCamera();
    SpawnPlayers();
}

void GameView::OnHide()
{
}

void GameView::CreateMusic()
{
    m_pMusic = CreateMusicEmitter();
    AddNode(m_pMusic);
}

void GameView::OnUpdate()
{
    UpdateTime();
    UpdateDanceSequence();
    UpdateMonsterSpawning();
    UpdateEntities();
    UpdateCamera();
}

void GameView::UpdateMonsterSpawning()
{
    if (GetTimeOfDay() != TimeOfDay::Night) return;

    float spawnRate = GetMonsterSpawnRate();
    m_monsterSpawnTime -= ODT;
    if (m_monsterSpawnTime <= 0.f)
    {
        m_monsterSpawnTime = spawnRate;
        SpawnMonster();
    }
}

void GameView::UpdateEntities()
{
    for (auto pEntity : m_entities)
    {
        pEntity->UpdateEntity();
    }
}

void GameView::UpdateCamera()
{
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
    }

    static const Color dayAmbient(1, 1, 1, 1);
    static const Color nightAmbient = Color(.10f, .15f, .2f, 1);
    static const Color dawnAmbient(1, .75f, 0, 1);
    static const Color duskAmbient(.75f, .35f, .55f, 1);

    auto timeOfDay = GetTimeOfDay();
    switch (timeOfDay)
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

    if (timeOfDay != m_previousTimeOfDay)
    {
        m_previousTimeOfDay = timeOfDay;
        OnTimeOfDayChanged(m_previousTimeOfDay);
    }
}

void GameView::OnTimeOfDayChanged(TimeOfDay timeOfDay)
{
    switch (timeOfDay)
    {
        case TimeOfDay::Night:
            StartDanceSequence();
            m_monsterSpawnTime = 0.f;
            break;
        case TimeOfDay::Day:
            break;
        case TimeOfDay::Dusk:   // soir commence
            m_pMusic->Play("RitualMusicAmbient.mp3", 1.f);
            break;
        case TimeOfDay::Dawn:   // matin commence
            m_pMusic->Stop(3.f);
            StopDanceSequence();
            m_day++;
            break;
    }
}

float GameView::GetDayTimeHour() const
{
    return (m_dayTime / DAY_TOTAL_DURATION) * 24.f;
}

static float lerpf(float from, float to, float t)
{
    return from + (to - from) * t;
}

float GameView::GetMonsterSpawnRate() const
{
    float nightPercent = (m_dayTime - DUSK_END) / NIGHT_DURATION;
    float ratePercent = 1.f;
    if (nightPercent < .5f)
    {
        ratePercent = lerpf(.25f, 1.f, nightPercent * 2.f);
    }
    else
    {
        ratePercent = lerpf(1.f, .25f, (nightPercent * 2.f) - 1.f);
    }
    return 1.f / (ratePercent * (float)m_day * .5f);
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
            player->Init(g_playerSpawn[i], this, i);
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

void GameView::StopDanceSequence()
{
    if (m_activeDanceSequence)
    {
        delete m_activeDanceSequence;
        m_activeDanceSequence = nullptr;
    }

    for (Player* p : m_players)
    {
        p->OnPedestralLockCancel();
    }
}

void GameView::UpdateDanceSequence()
{
    if (m_activeDanceSequence)
    {
        // gather each player inputs
        for (Player* p : m_players)
        {
            if (m_activeDanceSequence->PlayerNeedsToComplete(p->GetControllerIndex()))
            {
                if (m_activeDanceSequence->IsNailed(p->GetInputSequence(), p->GetControllerIndex()))
                {
                    // yay
                    p->OnDanceSequenceSuccess();
                }
            }
            //else
            //{
            //    m_activeDanceSequence->Skip();
            //}
        }
        
        if (m_activeDanceSequence->Update())
        {
            for (Player* p : m_players)
            {
               p->ResetInputSequence();
            }
        }
    }
}

void GameView::CreateTileMap()
{
    auto pTileMapNode = CreateTiledMapNode("maptemplate.tmx");
    m_pGameLayer->Attach(pTileMapNode, 0);

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
    Vector2 mapCenter((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);

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
            if (pos.x >= mapCenter.x - 3 && pos.y >= mapCenter.y - 3 &&
                pos.x <= mapCenter.x + 3 && pos.y <= mapCenter.y + 3) continue;
            auto pTile = GetTileAt(pos);
            if (!pTile) continue;
            if (pTile->isOccupied) continue;
            pTile->isOccupied = true;
            auto pTree = new Tree(this, pos);
            AddEntity(pTree);
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
            pos.x = std::round(pos.x) + .5f;
            pos.y = std::round(pos.y) + .5f;
            if (pos.x >= mapCenter.x - 3 && pos.y >= mapCenter.y - 3 &&
                pos.x <= mapCenter.x + 3 && pos.y <= mapCenter.y + 3) continue;
            auto pTile = GetTileAt(pos);
            if (!pTile) continue;
            if (pTile->isOccupied) continue;
            pTile->isOccupied = true;
            auto pRock = new Rock(this, pos);
            AddEntity(pRock);
        }
    }
}

void GameView::SpawnMonster()
{
    return;
    if (Monster::count >= MAX_MONSTER_COUNT) return;
    for (int tries = 0; tries < 10; ++tries)
    {
        int side = onut::randi() % 4;
        Vector2 spawnPos;
        switch (side)
        {
            case 0:
                spawnPos = onut::rand2f(Vector2::Zero, Vector2((float)m_pTilemap->getWidth(), 2.5f));
                break;
            case 1:
                spawnPos = onut::rand2f(Vector2::Zero, Vector2(2.5f, (float)m_pTilemap->getHeight()));
                break;
            case 2:
                spawnPos = onut::rand2f(Vector2((float)m_pTilemap->getWidth() - 2.5f, 0.f), Vector2(Vector2((float)m_pTilemap->getWidth(), (float)m_pTilemap->getHeight())));
                break;
            case 3:
                spawnPos = onut::rand2f(Vector2(0.f, (float)m_pTilemap->getHeight() - 2.5f), Vector2(Vector2((float)m_pTilemap->getWidth(), (float)m_pTilemap->getHeight())));
                break;
        }
        spawnPos.x = std::round(spawnPos.x) + .5f;
        spawnPos.y = std::round(spawnPos.y) + .5f;
        auto pTile = GetTileAt(spawnPos);
        if (!pTile) continue;
        if (!pTile->isOccupied)
        {
            auto pMonster = new Monster(MonsterType::CRAWLER, this, spawnPos);
            AddEntity(pMonster);
            break;
        }
    }
}

eTile GameView::GetTileIdAt(const Vector2& position) const
{
    return (eTile)m_pTilemap->getTileAt(m_pTileLayer, (int)position.x, (int)position.y);
}

Tile *GameView::GetTileAt(const Vector2& position) const
{
    auto x = (int)position.x;
    auto y = (int)position.y;
    return GetTileAt(x, y);
}

Tile* GameView::GetTileAt(int x, int y) const
{
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

    // 4 dance pedestral
    for (int i = 0; i < 4; ++i)
    {
        Vector2 pedOffset;
        if (i == 0) pedOffset = Vector2(-2, -2);
        if (i == 1) pedOffset = Vector2(2, -2);
        if (i == 2) pedOffset = Vector2(2, 2);
        if (i == 3) pedOffset = Vector2(-2, 2);
        DancePedestral* pedes = new DancePedestral(this, GetMapCenter() + pedOffset);
        m_pedestrals.push_back(pedes);
        AddEntity(pedes);
    }
    
}

void GameView::AddEntity(Entity* pEntity)
{
    m_pGameLayer->Attach(pEntity, (int)(pEntity->GetPosition().y * 16.f));
    m_entities.push_back(pEntity);
    auto pTile = GetTileAt(pEntity->GetPosition());
    if (pTile)
    {
        pTile->RegisterEntity(pEntity);
    }
}

void GameView::OnEntityMoved(Entity* pEntity)
{
    m_pGameLayer->Detach(pEntity);
    m_pGameLayer->Attach(pEntity, (int)(pEntity->GetPosition().y * 16.f));
    auto pTile = GetTileAt(pEntity->GetPosition());
    if (pTile)
    {
        pTile->RegisterEntity(pEntity);
    }
}

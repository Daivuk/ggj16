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
#include "BloodLayer.h"
#include "Stockpile.h"
#include "Stone.h"
#include "Scarecrow.h"

#define TREE_DENSITY 50
#define ROCK_DENSITY 30

const Vector2 g_playerSpawn[] = {
    {14.5f, 14.5f},
    {18.5f, 18.5f},
    {14.5f, 18.5f},
    {18.5f, 14.5f},
};

static float lerpf(float from, float to, float t)
{
    return from + (to - from) * t;
}

GameView* g_gameView = nullptr;

GameView::GameView()
{
    g_gameView = this;
    OUI->add(OLoadUI("../../assets/uis/game.json"));
    m_storeAnim = OFindUI("store")->rect.size.x;
    OFindUI("store")->rect.position.x = 140;
}

GameView::~GameView()
{
    if (m_pBloodLayer)
    {
        delete m_pBloodLayer;
    }
    if (m_pTiles)
    {
        delete[] m_pTiles;
    }
    if (m_pFireplace)
    {
        delete m_pFireplace;
    }
    if (m_pPather)
    {
        delete m_pPather;
    }
}

void GameView::OnShow()
{
    m_gameover = false;
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
    CreatePathFinder();
}

void GameView::OnHide()
{
    KillAllEntities();
    m_entities.clear();
    m_players.clear();
    m_pedestrals.clear();

    m_previousTimeOfDay = TimeOfDay::INVALID;
    m_dayTime = NOON;
    m_day = 1;
    m_monsterSpawnTime = 0;
    m_activeDanceSequence = nullptr;
    m_pMusic = nullptr;
}

void GameView::CreatePathFinder()
{
    m_pPather = new micropather::MicroPather(this);
}

void GameView::CreateMusic()
{
    m_pMusic = CreateMusicEmitter();
    AddNode(m_pMusic);
}

void GameView::OnUpdate()
{
    if (m_gameover)
    {
        if (!m_fadeQuad->GetColorAnim().isPlaying())
        {
            // we are done
            SendCommand(seed::eAppCommand::SWITCH_VIEW, "GameOverView");
        }
        return;
    }

    UpdateTime();
    UpdateDanceSequence();
    UpdateMonsterSpawning();
    UpdateEntities();
    UpdateCamera();
    UpdateUIs();

    // check if we are game over
    if (AllPlayersAreDead())
    {
        OnGameOver();
    }

    ClearEntities();
}

static std::unordered_map<StoreItemType, StoreItem> store = {
    {StoreItemType::Scarecrow, {StoreItemType::Scarecrow, {{DropType::Wood, 2}}, "scarecrow", OYBtn}},
    {StoreItemType::Stone, {StoreItemType::Stone, {{DropType::Rock, 2}}, "stone", OXBtn}}
};

void GameView::UpdateUIs()
{
    OFindUI("store")->rect.position.x = m_storeAnim;

    for (auto& storeItem : store)
    {
        bool canAfford = true;
        for (auto& kv : storeItem.second.cost)
        {
            if (m_pStockpile->resources[kv.first] < kv.second)
            {
                canAfford = false;
                break;
            }
        }
        OFindUI(storeItem.second.ui)->isVisible = canAfford;
    }
}

Entity* GameView::Buy(StoreItemType item)
{
    auto &storeItem = store[item];

    // Check if we can afford
    bool canAfford = true;
    for (auto& kv : storeItem.cost)
    {
        if (m_pStockpile->resources[kv.first] < kv.second)
        {
            canAfford = false;
            break;
        }
    }
    if (!canAfford) return nullptr;

    // Deplete resources
    for (auto& kv : storeItem.cost)
    {
        m_pStockpile->resources[kv.first] -= kv.second;
    }
    m_pStockpile->UpdateTexts();

    // Create a new entity, give it to player
    switch (item)
    {
        case StoreItemType::Scarecrow:
            return new Scarecrow(this);
            break;
        case StoreItemType::Stone:
            return new Stone(this);
            break;
    }

    return nullptr;
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


vector<Entity*> GameView::GetEntitiesInRadius(const Vector2& in_pos, float in_radius)
{
    vector<Entity*> res;
    Tile* tile = GetTileAt(in_pos);
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    // get all the tiles around it
    tile = GetTileAt(in_pos + Vector2(-1,0));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(-1, -1));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(0, -1));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(1, -1));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(1, 0));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(1, 1));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(0, 1));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }

    tile = GetTileAt(in_pos + Vector2(-1, 1));
    if (tile)
    {
        FillVectorWithEntitiesInRadius(tile, in_pos, in_radius, res);
    }
    return res;
}

void GameView::FillVectorWithEntitiesInRadius(Tile* in_tile, const Vector2& in_pos, float in_radius, vector<Entity*>& inOut_result)
{
    for (auto entity = in_tile->pEntities->Head(); entity; entity = in_tile->pEntities->Next(entity))
    {
        if ((entity->GetPosition() - in_pos).LengthSquared() < in_radius * in_radius)
        {
            inOut_result.push_back(entity);
        }
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
    // Fit players in view
    Vector2 minPlayer((float)m_pTilemap->getWidth(), (float)m_pTilemap->getHeight());
    Vector2 maxPlayer(0.f);

    if (m_players.empty()) return;

    for (auto pPlayer : m_players)
    {
        minPlayer.x = onut::min(pPlayer->GetPosition().x, minPlayer.x);
        minPlayer.y = onut::min(pPlayer->GetPosition().y, minPlayer.y);
        maxPlayer.x = onut::max(pPlayer->GetPosition().x, maxPlayer.x);
        maxPlayer.y = onut::max(pPlayer->GetPosition().y, maxPlayer.y);
    }

    minPlayer.x -= 3;
    minPlayer.y -= 3;
    maxPlayer.x += 3;
    maxPlayer.y += 3;

    m_camera = (minPlayer + maxPlayer) * .5f;

    float playerViewHeight = maxPlayer.y - minPlayer.y;
    float zoomH = OScreenHf / playerViewHeight;
    if (zoomH > 64.f) zoomH = 64.f;

    float playerViewWidth = maxPlayer.x - minPlayer.x;
    float zoomW = OScreenWf / playerViewWidth;
    if (zoomW > 64.f) zoomW = 64.f;

    m_zoom = onut::min(zoomW, zoomH);

    // Animate to target position
    m_cameraReal = Vector2::Lerp(m_cameraReal, m_camera, ODT * 1.5f);
    m_zoomReal = lerpf(m_zoomReal, m_zoom, ODT * 4.f);

    // Update camera based on the players position
    GetRootNode()->SetScale(Vector2(m_zoomReal));
    GetRootNode()->SetPosition(-m_cameraReal * m_zoomReal + OScreenf * .5f);
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
            m_pMusic->Play("RitualMusicAmbient.mp3");
            break;
        case TimeOfDay::Dawn:   // matin commence
            KillAllMonsters();
            m_pMusic->Stop(3.f);
            StopDanceSequence();
            m_day++;
            break;
    }
}

void GameView::KillAllEntities()
{
    m_entitiesToKill = m_entities;
    ClearEntities();
}

void GameView::KillEntity( Entity* in_toKill )
{
    for (auto pEntity : m_entitiesToKill) if (pEntity == in_toKill) return;
    m_entitiesToKill.push_back(in_toKill);
}

void GameView::ClearEntities()
{
    for (size_t i = 0; i < m_entitiesToKill.size(); ++i)
    {
        for (auto it = m_scarecrows.begin(); it != m_scarecrows.end();)
        {
            if ((*it) == m_entitiesToKill[i])
            {
                it = m_scarecrows.erase(it);
                break;
            }
            else
            {
                ++it;
            }
        }
        for (auto it = m_entities.begin(); it != m_entities.end();)
        {
            if ((*it) == m_entitiesToKill[i])
            {
                it = m_entities.erase(it);
                DeleteNode(m_entitiesToKill[i]);
                break;
            }
            else
            {
                ++it;
            }
        }
    }
    m_entitiesToKill.clear();

    for (auto pEntity : m_entitiesToAdd)
    {
        m_pGameLayer->Attach(pEntity, (int)(pEntity->GetPosition().y * 16.f));
        m_entities.push_back(pEntity);
        auto pTile = GetTileAt(pEntity->GetPosition());
        if (pTile)
        {
            pTile->RegisterEntity(pEntity);
        }
        if (dynamic_cast<Scarecrow*>(pEntity))
        {
            m_scarecrows.push_back(pEntity);
        }
    }
    m_entitiesToAdd.clear();
}

void GameView::KillAllMonsters()
{
    for (auto it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        auto pMonster = dynamic_cast<Monster*>(*it);
        if (pMonster)
        {
            KillEntity(pMonster);
        }
    }
}

float GameView::GetDayTimeHour() const
{
    return (m_dayTime / DAY_TOTAL_DURATION) * 24.f;
}

float GameView::GetNightPercent() const
{
    if (m_dayTime < DAWN_START)
    {
        return ((DAY_TOTAL_DURATION - DUSK_END) + m_dayTime) / NIGHT_DURATION;
    }
    else
    {
        return (m_dayTime - DUSK_END) / NIGHT_DURATION;
    }
}

float GameView::GetMonsterSpawnRate() const
{
    float nightPercent = GetNightPercent();
    float ratePercent = 1.f;
    if (nightPercent < .5f)
    {
        ratePercent = lerpf(.25f, 1.f, nightPercent * 2.f);
    }
    else
    {
        ratePercent = lerpf(1.f, .25f, (nightPercent * 2.f) - 1.f);
    }
    return 1.f / ((ratePercent) * (float)m_day * .5f);
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
    if (m_gameover)
        return;

#if defined(_DEBUG)
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
    pFont->draw("Monster spawn Rate: " + std::to_string(GetMonsterSpawnRate()), Vector2(0, 40));
    pFont->draw("Monster count: " + std::to_string(Monster::count), Vector2(0, 60));
    pFont->draw("Day: " + std::to_string(m_day), Vector2(0, 80));

#if 0 // Show path
    OSB->end();
    for (auto pEntity : m_entities)
    {
        auto pMonster = dynamic_cast<Monster*>(pEntity);
        if (pMonster)
        {
            OPB->begin(onut::ePrimitiveType::LINE_STRIP, nullptr, GetRootNode()->GetTransform());
            for (auto& p : pMonster->m_path)
            {
                OPB->draw(p);
            }
            OPB->end();
        }
    }
    OSB->begin();
#endif
#endif
}

void GameView::SpawnPlayers()
{
    for (int i = 0; i < 4; ++i)
    {
        if (g_activePlayer[i])
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
    // activate pods
    for (DancePedestral* d : m_pedestrals)
    {
        d->StartEnabledFX();
    }

    m_activeDanceSequence = new DanceSequence();
    m_activeDanceSequence->Init(m_day, m_pFireplace, this);
}

void GameView::StopDanceSequence()
{
    // activate pods
    for (DancePedestral* d : m_pedestrals)
    {
        d->StopFXes();
    }

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

                    // grow the fire
                    GrowFire();
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
    m_pGameLayer->Attach(pTileMapNode, -10);

    m_pTilemap = pTileMapNode->GetTiledMap();
    m_pBackgroundLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("backgrounds");
    m_pTileLayer = (onut::TiledMap::sTileLayer*)m_pTilemap->getLayer("tiles");

    pTileMapNode->SetScale(Vector2(1.f / m_pTilemap->getTileWidth()));

    m_pTiles = new Tile[m_pTilemap->getWidth() * m_pTilemap->getHeight()];

    m_pGameLayer->Attach(m_pBloodLayer = new BloodLayer(m_pTilemap->getWidth(), m_pTilemap->getHeight()), -11);
}

void GameView::CenterCamera()
{
    m_cameraReal = m_camera = Vector2((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);
}

void GameView::GenerateMap()
{
    Vector2 mapCenter((float)m_pTilemap->getWidth() * .5f, (float)m_pTilemap->getHeight() * .5f);

    m_pStockpile = new Stockpile(this, m_pTilemap->getWidth() / 2 + 1, m_pTilemap->getHeight() / 2 - 4);
    AddEntity(m_pStockpile);

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

void GameView::SetTileIdAt(const Vector2& position, eTile tileId)
{
    m_pTilemap->setTileAt(m_pTileLayer, (int)position.x, (int)position.y, (int)tileId);
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
        DancePedestral* pedes = new DancePedestral(this, GetMapCenter() + pedOffset, i);
        m_pedestrals.push_back(pedes);
        AddEntity(pedes);
    }
}

void GameView::AddEntity(Entity* pEntity)
{
    m_entitiesToAdd.push_back(pEntity);
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

void GameView::GrowFire()
{
    m_pFireplace->Grow();
}

void GameView::OnGameOver()
{
    StopDanceSequence();
    m_gameover = true;

    // start fading the quad anim
    m_fadeQuad = CreateSprite("healthGaugeFill.png");
    m_fadeQuad->GetColorAnim().start(Color(0.f, 0.f, 0.f, 0.f), Color(0.f, 0.f, 0.f, 1.f), 5.f);
    m_fadeQuad->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_fadeQuad->SetScale(Vector2(100, 100));
    AddNode(m_fadeQuad);
}

bool GameView::AllPlayersAreDead()
{
    for (Player* p : m_players)
    {
        if (p->IsAlive())
            return false;
    }
    return true;
}

Player* GameView::GetClosestPlayer(const Vector2& position) const
{
    float closestDist = 100000.f;
    Player* pRet = nullptr;
    for (auto pPlayer : m_players)
    {
        if (!pPlayer->IsAlive())
            continue;

        float dist = Vector2::DistanceSquared(pPlayer->GetPosition(), position);
        if (dist < closestDist)
        {
            closestDist = dist;
            pRet = pPlayer;
        }
    }
    return pRet;
}

Entity* GameView::GetClosestPlayerAsSeenByMonster(const Vector2& position) const
{
    float closestDist = 100000.f;
    Entity* pRet = nullptr;
    for (auto pPlayer : m_players)
    {
        if (!pPlayer->IsAlive())
            continue;

        float dist = Vector2::DistanceSquared(pPlayer->GetPosition(), position);
        if (dist < closestDist)
        {
            closestDist = dist;
            pRet = pPlayer;
        }
    }
    for (auto pPlayer : m_scarecrows)
    {
        float dist = Vector2::DistanceSquared(pPlayer->GetPosition(), position);
        if (dist < closestDist)
        {
            closestDist = dist;
            pRet = pPlayer;
        }
    }
    return pRet;
}

void GameView::ShowStore()
{
    m_storeAnim.stop(false);
    m_storeAnim.startFromCurrent(0.f, .25f, OSpringOut);
}

void GameView::HideStore()
{
    m_storeAnim.stop(false);
    m_storeAnim.startFromCurrent(OFindUI("store")->rect.size.x, .25f, OEaseIn);
}

void GameView::OnPlayerSacrifice(Player* in_player)
{
    for (Player* p : m_players)
    {
        if (p != in_player)
        {
            p->RestoreFullHealth();
        }
    }
    KillAllMonsters();
    m_pFireplace->GrowToMax();
}

void GameView::SplatGore(const Vector2& pos)
{
    m_pBloodLayer->SplatGore(pos);
}

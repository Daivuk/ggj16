#pragma once
#include "View.h"
#include "Globals.h"
#include "onut.h"

enum eTile : uint32_t
{
    TILE_NONE = 0,
    TILE_GRASS,
    TILE_TREE,
    TILE_ROCK,
    TILE_FIREPLACE
};

enum class TimeOfDay
{
    INVALID,
    Night,
    Dawn,
    Day,
    Dusk
};

class DanceSequence;
class Fireplace;
class Tile;
class DancePedestral;

class GameView : public seed::View
{
public:
    GameView();
    virtual ~GameView();

	virtual void OnShow();
	virtual void OnHide();
    virtual void OnUpdate();
    virtual void OnRender();

    TimeOfDay   GetTimeOfDay() const;
    float       GetDayTime() const { return m_dayTime; }
    float       GetDayTimeHour() const;
    int         GetDay() const { return m_day; }
    float       GetMonsterSpawnRate() const;

    eTile       GetTileIdAt(const Vector2& position) const;
    Tile*       GetTileAt(const Vector2& position) const;
    Tile*       GetTileAt(int x, int y) const;
    Vector2     GetMapCenter() const;

    void        OnEntityMoved(Entity* pEntity);

    vector<DancePedestral*>& GetPedestrals() { return m_pedestrals; }

private:
    PlayerVect  m_players;      // index 0 = player 1, etc

    void CreateMusic();
    void SpawnPlayers();
    void CreateTileMap();
    void CenterCamera();
    void GenerateMap();
    void CreateEntities();
    void SpawnMonster();

    void UpdateTime();
    void UpdateEntities();
    void UpdateCamera();
    void UpdateDanceSequence();
    void UpdateMonsterSpawning();

    void AddEntity(Entity* pEntity);
    void StartDanceSequence();
    void StopDanceSequence();
    void OnTimeOfDayChanged(TimeOfDay timeOfDay);

    seed::LightLayer* m_pGameLayer = nullptr;
    onut::TiledMap* m_pTilemap = nullptr;
    onut::TiledMap::sTileLayer* m_pBackgroundLayer = nullptr;
    onut::TiledMap::sTileLayer* m_pTileLayer = nullptr;
    Vector2 m_camera;
    float m_zoom = 64.f;
    Fireplace* m_pFireplace = nullptr;
    Tile *m_pTiles = nullptr;
    seed::MusicEmitter* m_pMusic = nullptr;

    DanceSequence*  m_activeDanceSequence = nullptr;
    int             m_difficulty = 1;
    
    EntityVect      m_entities;

    TimeOfDay       m_previousTimeOfDay = TimeOfDay::INVALID;
    float           m_dayTime = DUSK_START;
    int             m_day = 1;
    vector<DancePedestral*> m_pedestrals;
    float           m_monsterSpawnTime = 0.f;
};

extern GameView* g_gameView;

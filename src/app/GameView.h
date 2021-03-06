#pragma once
#include "View.h"
#include "Globals.h"
#include "onut.h"
#include "micropather.h"

enum eTile : uint32_t
{
    TILE_NONE = 0,
    TILE_STONE0 = 20,
    TILE_STONE1 = 21,
    TILE_STONE2 = 22,
    TILE_STONE3 = 23,
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
class Monster;
class BloodLayer;
class Stockpile;

enum class StoreItemType
{
    Scarecrow,
    Stone
};

struct StoreItem
{
    StoreItemType type;
    std::unordered_map<DropType, int> cost;
    std::string ui;
    onut::GamePad::eGamePad buyButton;
};

class GameView : public seed::View, public micropather::Graph
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
    float       GetNightPercent() const;

    void        SetTileIdAt(const Vector2& position, eTile tileId);
    eTile       GetTileIdAt(const Vector2& position) const;
    Tile*       GetTileAt(const Vector2& position) const;
    Tile*       GetTileAt(int x, int y) const;
    Vector2     GetMapCenter() const;

    void        OnEntityMoved(Entity* pEntity);

    Player*     GetClosestPlayer(const Vector2& position) const;
    Entity*     GetClosestPlayerAsSeenByMonster(const Vector2& position) const;

    vector<DancePedestral*>& GetPedestrals() { return m_pedestrals; }
    vector<Entity*>          GetEntitiesInRadius(const Vector2& in_pos, float in_radius);

    void        OnGameOver();

    // A* stuff
    float LeastCostEstimate(void* stateStart, void* stateEnd) override;
    void AdjacentCost(void* state, MP_VECTOR< micropather::StateCost > *adjacent) override;
    void PrintStateInfo(void* state) override;
    int findPath(const Vector2 &from, const Vector2 &to, std::vector<Vector2> *path, float* cost);
    void nodeToXY(void* node, int* x, int* y);
    void* xyToNode(int x, int y);
    bool passable(int x, int y);

    void KillEntity(Entity* in_toKill);
    void AddEntity(Entity* pEntity);

    const PlayerVect& GetPlayers() const { return m_players; }
    const EntityVect& GetScarecrows() const { return m_scarecrows; }

    void ShowStore();
    void HideStore();

    Fireplace* GetFireplace() { return m_pFireplace; }
    void OnPlayerSacrifice(Player* in_player);
    Stockpile* GetStockpile() const { return m_pStockpile; }
    Entity* Buy(StoreItemType item);

    void SplatGore(const Vector2& pos);

    int GetMapWidth() const { return m_pTilemap->getWidth(); }
    int GetMapHeight() const { return m_pTilemap->getHeight(); }

    int GetAlivePlayerCount();

private:
    PlayerVect  m_players;      // index 0 = player 1, etc

    void CreateMusic();
    void SpawnPlayers();
    void CreateTileMap();
    void CenterCamera();
    void GenerateMap();
    void CreateEntities();
    void SpawnMonster();
    void CreatePathFinder();

    void UpdateTime();
    void UpdateEntities();
    void UpdateCamera();
    void UpdateDanceSequence();
    void UpdateMonsterSpawning();
    void UpdateUIs();

    void ClearEntities();
    void KillAllEntities();

    void StartDanceSequence();
    void StopDanceSequence();
    void OnTimeOfDayChanged(TimeOfDay timeOfDay);
    void KillAllMonsters();

    seed::LightLayer* m_pGameLayer = nullptr;
    onut::TiledMap* m_pTilemap = nullptr;
    onut::TiledMap::sTileLayer* m_pBackgroundLayer = nullptr;
    onut::TiledMap::sTileLayer* m_pTileLayer = nullptr;
    Vector2 m_camera;
    Vector2 m_cameraReal;
    float m_zoom = 64.f;
    float m_zoomReal = 64.f;
    Fireplace* m_pFireplace = nullptr;
    Tile *m_pTiles = nullptr;
    seed::MusicEmitter* m_pMusic = nullptr;

    DanceSequence*  m_activeDanceSequence = nullptr;
    
    EntityVect      m_entities;
    EntityVect      m_scarecrows;
    
    TimeOfDay       m_previousTimeOfDay = TimeOfDay::INVALID;
    float           m_dayTime = NOON;
    int             m_day = 1;
    vector<DancePedestral*> m_pedestrals;
    float           m_monsterSpawnTime = 0.f;
    
    void        GrowFire();
    bool        m_gameover = false;
    BloodLayer* m_pBloodLayer = nullptr;
    Stockpile*  m_pStockpile = nullptr;
    
    micropather::MicroPather *m_pPather = nullptr;

    void FillVectorWithEntitiesInRadius(Tile* in_tile, const Vector2& in_pos, float in_radius, vector<Entity*>& inOut_result);

    vector<Entity*> m_entitiesToKill;
    vector<Entity*> m_entitiesToAdd;
    OAnimf          m_storeAnim;

    bool AllPlayersAreDead();

    seed::Sprite*       m_fadeQuad = nullptr;

    void PlayRandomDayMusic();

};

extern GameView* g_gameView;

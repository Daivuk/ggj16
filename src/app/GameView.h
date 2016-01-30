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


class DanceSequence;
class Fireplace;
class Tile;

class GameView : public seed::View
{
public:
    GameView();
    virtual ~GameView();

	virtual void OnShow();
	virtual void OnHide();
    virtual void OnUpdate();
    virtual void OnRender();

private:
    PlayerVect  m_players;      // index 0 = player 1, etc

    void SpawnPlayers();
    void CreateTileMap();
    void CenterCamera();
    void GenerateMap();
    void CreateEntities();

    void AddEntity(Entity* pEntity);

    eTile GetTileIdAt(const Vector2& position) const;
    Tile *GetTileAt(const Vector2& position) const;
    Vector2 GetMapCenter() const;

    seed::LightLayer* pGameLayer = nullptr;
    onut::TiledMap* m_pTilemap = nullptr;
    onut::TiledMap::sTileLayer* m_pBackgroundLayer = nullptr;
    onut::TiledMap::sTileLayer* m_pTileLayer = nullptr;
    Vector2 m_camera;
    float m_zoom = 64.f;
    Fireplace* m_pFireplace = nullptr;
    Tile *m_pTiles = nullptr;

    DanceSequence*  m_activeDanceSequence = nullptr;
    int             m_difficulty = 1;
    void StartDanceSequence();
    void UpdateDanceSequence();
    
    EntityVect      m_entities;
};
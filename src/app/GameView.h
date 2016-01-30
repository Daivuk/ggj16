#pragma once
#include "View.h"
#include "Globals.h"
#include "onut.h"

enum eTile
{
    TILE_NONE = 0,
    TILE_GRASS,
    TILE_TREE,
    TILE_ROCK,
    TILE_FIREPLACE
};

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
    void UpdatePlayers();
    void CreateTileMap();
    void CenterCamera();
    void GenerateMap();
    eTile GetTileAt(const Vector2& position);

    onut::TiledMap* m_pTilemap = nullptr;
    onut::TiledMap::sTileLayer* m_pBackgroundLayer = nullptr;
    onut::TiledMap::sTileLayer* m_pTileLayer = nullptr;
    Vector2 m_camera;
    float m_zoom = 32.f;
};
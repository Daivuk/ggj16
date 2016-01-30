#pragma once
#include "View.h"
#include "onut.h"

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
    void CreateTileMap();
    void CenterCamera();

    onut::TiledMap* m_pTilemap = nullptr;
    onut::TiledMap::sTileLayer* m_pTileLayer = nullptr;
    Vector2 m_camera;
    float m_zoom = 64.f;
};
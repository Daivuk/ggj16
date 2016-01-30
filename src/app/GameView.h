#pragma once
#include "View.h"
#include "Globals.h"

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
};
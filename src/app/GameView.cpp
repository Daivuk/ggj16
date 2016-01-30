#include "GameView.h"
#include "Player.h"

GameView::GameView()
{
}

GameView::~GameView()
{
}

void GameView::OnShow()
{
    // spawn players from the lobby data, for now assume one
    SpawnPlayers();
}

void GameView::OnHide()
{
}

void GameView::OnUpdate()
{
    UpdatePlayers();
}

void GameView::OnRender()
{
}

void GameView::SpawnPlayers()
{
    Player* player1 = new Player();
    // todo pass in what "skin" used
    player1->Init(Vector2(OScreenWf * .5f, OScreenHf * .5f), this, 1);
    m_players.push_back(player1);

    // todo spawn other players
}

void GameView::UpdatePlayers()
{
    for (Player* p : m_players)
    {
        p->Update();
    }
}

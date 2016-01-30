#include "Game.h"

// game views
#include "SplashView.h"
#include "StartView.h"
#include "GameView.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::OnStart()
{
	// create game specific views
    AddView("SplashView", new SplashView());
    AddView("StartView", new StartView());
    AddView("GameView", new GameView());

	// show the default view
	PushView("GameView");
}

void Game::OnUpdate()
{
}

void Game::OnRender()
{

}

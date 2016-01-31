#include "Game.h"

// game views
#include "SplashView.h"
#include "StartView.h"
#include "GameView.h"
#include "GameOverView.h"

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
    AddView("GameOverView", new GameOverView());

	// show the default view
	PushView("SplashView");
}

void Game::OnUpdate()
{
}

void Game::OnRender()
{

}

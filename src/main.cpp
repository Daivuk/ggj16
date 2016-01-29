// Required for WinMain
#include <Windows.h>

// Oak Nut include
#include "onut.h"

#include "app/Game.h"

void init();
void update();
void render();

static Game* s_runningApp = nullptr;

// Main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    // Set default settings
    OSettings->setBorderlessFullscreen(false);
    OSettings->setGameName("Global Game Jam 2016");
    OSettings->setIsFixedStep(false);
    OSettings->setIsResizableWindow(false);
    OSettings->setResolution({1280, 720});

    // Run
    ORun(init, update, render);
}

void init()
{
	s_runningApp = new Game();
	s_runningApp->Start();
}

void update()
{
	s_runningApp->Update();
}

void render()
{
	s_runningApp->Render();
}

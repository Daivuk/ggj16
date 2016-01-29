#pragma once
#include "App.h"

class Game : public seed::App
{
public:

    Game();
    virtual ~Game();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnRender();

private:
	

};
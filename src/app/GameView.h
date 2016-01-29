#pragma once
#include "View.h"

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
};
#pragma once
#include "View.h"

class Button;
class StartView : public seed::View
{
public:

	StartView();
	virtual ~StartView();

	virtual void OnShow();
	virtual void OnHide();
    virtual void OnUpdate() override;
	
private:
    void CreateDude(seed::Node* pParent, int id);
    seed::Sprite* m_dudes[4];
    bool m_activePlayer[4];
};
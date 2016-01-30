#pragma once
#include "Sprite.h"
#include "View.h"
#include "Entity.h"

#include <string>

class Player : public Entity
{
public:

    Player();
    virtual ~Player();

    void Init(const Vector2& in_position, seed::View* in_container, int in_controllerIndex);
	void OnRender();
    void Update() override;

private:

    seed::View*     m_container = nullptr;
    seed::Sprite*   m_sprite = nullptr;
    int             m_controllerIndex = -1;
    std::string     m_idleAnim;
    Vector2         m_thumb;
    Vector2         m_vel;

    void    UpdateInputs();
    void    UpdateVel();
    void    UpdateSpriteAnim();

};
#pragma once
#include "Entity.h"
#include <unordered_map>
#include "Globals.h"

namespace seed
{
    class SpriteString;
};

class Stockpile : public Entity
{
public:
    Stockpile(seed::View* pView, int x, int y);
    virtual ~Stockpile();

    virtual float   GetWidth() const { return 2.f; }
    virtual float   GetHeight() const { return 1.f; }

    virtual void UpdateEntity() override;

    std::unordered_map<DropType, int> resources;

private:
    seed::View* m_pView = nullptr;
    seed::Sprite* m_pRockSprite = nullptr;
    seed::Sprite* m_pWoodSprite = nullptr;
    seed::SpriteString* m_pRockText = nullptr;
    seed::SpriteString* m_pWoodText = nullptr;

    bool m_bSomeoneAround = false;
};

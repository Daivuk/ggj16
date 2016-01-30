#pragma once
#include "Entity.h"

class Stockpile : public Entity
{
public:
    Stockpile(seed::View* pView, int x, int y);
    virtual ~Stockpile();

    virtual float   GetWidth() const { return 2.f; }
    virtual float   GetHeight() const { return 1.f; }

private:
    seed::View* m_pView = nullptr;
};

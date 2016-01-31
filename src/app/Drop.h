#pragma once
#include "Entity.h"
#include "Globals.h"

class Drop : public Entity
{
public:
    Drop(seed::View* pView, DropType in_type);

    DropType type;

private:
    seed::View* m_pView = nullptr;
};

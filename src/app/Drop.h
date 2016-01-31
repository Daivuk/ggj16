#pragma once
#include "Entity.h"

enum class DropType
{
    Wood,
    Rock,
};

class Drop : public Entity
{
public:
    Drop(seed::View* pView, DropType in_type);

    DropType type;

private:
    seed::View* m_pView = nullptr;
};

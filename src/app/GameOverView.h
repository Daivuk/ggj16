#pragma once
#include "View.h"

class GameOverView final : public seed::View
{
public:
    void OnShow() override;
    void OnUpdate() override;

private:
    OTimer m_timer;
};

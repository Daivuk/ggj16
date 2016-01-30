#include "DancePedestral.h"
#include "Globals.h"

DancePedestral::DancePedestral(seed::View* pView, const Vector2& position, int index)
{
    m_position = position;

    auto dancePed = pView->CreateSprite("dancePedestral" + std::to_string(index) + ".png");
    Attach(dancePed);
    dancePed->SetScale(Vector2(SPRITE_SCALE));
    dancePed->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
}

DancePedestral::~DancePedestral()
{
}

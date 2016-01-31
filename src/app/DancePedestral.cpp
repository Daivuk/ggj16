#include "DancePedestral.h"
#include "Globals.h"

DancePedestral::DancePedestral(seed::View* pView, const Vector2& position, int index)
{
    m_position = position;

    auto dancePed = pView->CreateSprite("dancePedestral" + std::to_string(index) + ".png");
    Attach(dancePed);
    dancePed->SetScale(Vector2(SPRITE_SCALE));
    dancePed->SetFilter(onut::SpriteBatch::eFiltering::Nearest);

    m_baseFX = pView->CreateEmitter("DancePodFX.pex");
    m_baseFX->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_baseFX->SetEmitWorld(false);
    m_baseFX->SetScale(Vector2(SPRITE_SCALE) * .2f);
    Attach(m_baseFX);

    m_activatedFX = pView->CreateEmitter("DancePodActivatedFX.pex");
    m_activatedFX->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_activatedFX->SetEmitWorld(false);
    m_activatedFX->SetScale(Vector2(SPRITE_SCALE) * .2f);
    Attach(m_activatedFX);
}

DancePedestral::~DancePedestral()
{
}

void DancePedestral::StartEnabledFX()
{
    StopFXes();
    m_baseFX->Start();
}

void DancePedestral::StartActivatedFX()
{
    StopFXes();
    m_activatedFX->Start();
}

void DancePedestral::StopFXes()
{
    m_baseFX->Stop();
    m_activatedFX->Stop();
}
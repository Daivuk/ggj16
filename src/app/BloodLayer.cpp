#include "BloodLayer.h"
#include "Globals.h"

BloodLayer::BloodLayer(int width, int height)
    : m_width(width)
    , m_height(height)
{
    m_pGoreTexture = OTexture::createRenderTarget({m_width * 16, m_height * 16});
}

BloodLayer::~BloodLayer()
{
    if (m_pGoreTexture)
    {
        delete m_pGoreTexture;
    }
}

void BloodLayer::Render(Matrix* in_parentMatrix, float in_parentAlpha)
{
    // render ourself
    OSB->end();
    OSB->begin(*in_parentMatrix, onut::SpriteBatch::eBlendMode::Multiplied);
    OSpriteBatch->changeFiltering(onut::SpriteBatch::eFiltering::Nearest);
    OSpriteBatch->drawRect(m_pGoreTexture, Rect(0, 0, m_pGoreTexture->getSizef() * SPRITE_SCALE));
    OSB->end();
    OSB->begin();
}

void BloodLayer::SplatGore(const Vector2& pos)
{
    m_pGoreTexture->bindRenderTarget();
    auto pTexture = OGetTexture(("blood" + std::to_string(onut::randi() % 4) + ".png").c_str());
    OSB->begin();
    pTexture = OGetTexture(("blood" + std::to_string(onut::randi() % 4) + ".png").c_str());
    OSB->drawSprite(pTexture, pos * 16.f + Vector2(-8.f, -4.f), Color(1, 1, 1, .5f), onut::randf(360.f));
    pTexture = OGetTexture(("blood" + std::to_string(onut::randi() % 4) + ".png").c_str());
    OSB->drawSprite(pTexture, pos * 16.f + Vector2(4.f, 8.f), Color(1, 1, 1, .5f), onut::randf(360.f));
    pTexture = OGetTexture(("blood" + std::to_string(onut::randi() % 4) + ".png").c_str());
    OSB->drawSprite(pTexture, pos * 16.f + Vector2(6.f, -6.f), Color(1, 1, 1, .5f), onut::randf(360.f));
    pTexture = OGetTexture(("blood" + std::to_string(onut::randi() % 4) + ".png").c_str());
    OSB->drawSprite(pTexture, pos * 16.f, Color(1, 1, 1, .75f), onut::randf(360.f));
    OSB->end();
    m_pGoreTexture->unbindRenderTarget();
}

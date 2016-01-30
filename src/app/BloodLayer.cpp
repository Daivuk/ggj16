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
    OSB->begin(*in_parentMatrix, onut::SpriteBatch::eBlendMode::Alpha);
    OSpriteBatch->changeFiltering(onut::SpriteBatch::eFiltering::Nearest);
    OSpriteBatch->drawRect(m_pGoreTexture, Rect(0, 0, m_pGoreTexture->getSizef() * SPRITE_SCALE));
    OSB->end();
    OSB->begin();
}

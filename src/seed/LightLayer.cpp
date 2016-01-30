#include "LightLayer.h"
#include "onut.h"

namespace seed
{
    LightLayer::LightLayer()
        : m_lightTarget(nullptr)
    {
    }

    LightLayer::~LightLayer()
    {
        if (m_lightTarget)
        {
            delete m_lightTarget;
        }
    }

    void LightLayer::Render(Matrix* in_parentMatrix, float in_parentAlpha)
    {
        if (!m_visible)
        {
            return;
        }

        // generate our matrix
        Matrix transform = Matrix::Identity;
        transform *= Matrix::CreateScale(m_scale.get().x, m_scale.get().y, 1.f);
        transform *= Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_angle));
        transform *= Matrix::CreateTranslation(m_position.get().x, m_position.get().y, 0);

        if (in_parentMatrix)
        {
            transform = transform * *in_parentMatrix;
        }

        // render bg children
        RenderChildren(m_bgChildren, &transform, m_color.get().w * in_parentAlpha);

        // render fg children
        RenderChildren(m_fgChildren, &transform, m_color.get().w * in_parentAlpha);

        if (!m_lightTarget)
        {
            m_lightTarget = OTexture::createScreenRenderTarget(true);
        }

        bool scissorEnabled = ORenderer->getScissorEnabled();
        Rect scissorRect = ORenderer->getScissor();

        {
            OSB->end();
            const Matrix& spriteBatchTransform = OSB->getTransform();
            m_lightTarget->bindRenderTarget();
            m_lightTarget->clearRenderTarget(m_ambient);
            OSB->begin(spriteBatchTransform, onut::SpriteBatch::eBlendMode::Add);
            OSB->changeFiltering(onut::SpriteBatch::eFiltering::Linear);
            if (scissorEnabled)
            {
                ORenderer->setScissor(scissorEnabled, scissorRect);
            }
        }

        // render bg children
        RenderChildrenLight(m_bgChildren, &transform, m_color.get().w * in_parentAlpha);

        // render fg children
        RenderChildrenLight(m_fgChildren, &transform, m_color.get().w * in_parentAlpha);

        {
            OSB->end();
            Matrix spriteBatchTransform = OSB->getTransform();
            m_lightTarget->unbindRenderTarget();

            OSB->begin(onut::SpriteBatch::eBlendMode::Multiplied);
            if (scissorEnabled)
            {
                ORenderer->setScissor(scissorEnabled, scissorRect);
            }
            OSB->drawRect(m_lightTarget, ORectFullScreen);
            OSB->end();
            OSB->begin(spriteBatchTransform);
            if (scissorEnabled)
            {
                ORenderer->setScissor(scissorEnabled, scissorRect);
            }
        }
    }
}

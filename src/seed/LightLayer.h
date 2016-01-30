#pragma once

#include "Node.h"

namespace seed
{
    class LightLayer : public Node
    {
    public:
        LightLayer();
        virtual ~LightLayer();

        void SetAmbient(const Color& ambient) { m_ambient = ambient; }
        const Color& GetAmbient() const { return m_ambient; }

        // only to be used by the seed sdk
        virtual void    Render(Matrix* in_parentMatrix = nullptr, float in_parentAlpha = 1.f) override;

    protected:

        OTexture*       m_lightTarget;
        Color           m_ambient = Color(.3f, .3f, .4f, 1.f);
    };
}

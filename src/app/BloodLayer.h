#pragma once
#include "Node.h"

class BloodLayer : public seed::Node
{
public:
    BloodLayer(int width, int height);
    virtual ~BloodLayer();

    virtual void Render(Matrix* in_parentMatrix = nullptr, float in_parentAlpha = 1.f) override;
    void SplatGore(const Vector2& pos);

private:
    OTexture* m_pGoreTexture = nullptr;
    int m_width;
    int m_height;
};

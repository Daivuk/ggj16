#include "seed/Node.h"
#include "SplashView.h"
#include "SpriteString.h"

void SplashView::OnShow()
{
    Load("../../assets/views/splashview.xml");

    auto pLogo = FindNode("logo");
    auto scale = pLogo->GetScale();
    pLogo->GetScaleAnim().startKeyframed(
        Vector2(scale.x, 0.f),
        {
            OAnimWait(Vector2(scale.x, 0.f), .5f),
            {scale, .1f, OEaseIn},
            OAnimWait(scale, 1.f),
            {Vector2(scale.x * 1.2f, scale.y * .6f), .02f, OEaseOut},
            {scale, .02f, OEaseIn},
            OAnimWait(scale, .5f),
            {Vector2(scale.x * 1.1f, scale.y * .8f), .02f, OEaseOut},
            {scale, .02f, OEaseIn},
            OAnimWait(scale, .75f),
            {Vector2(scale.x * 1.1f, scale.y * .65f), .02f, OEaseOut},
            {scale, .02f, OEaseIn},
            OAnimWait(scale, .75f),
            {Vector2(scale.x * 1.2f, scale.y * .7f), .02f, OEaseOut},
            {scale, .02f, OEaseIn},
            {Vector2(scale.x * .85f, .01f), .20f, OEaseOut},
            {Vector2(.01f, .01f), .1f, OEaseOut, [this]
            {
                SendCommand(seed::eAppCommand::SWITCH_VIEW, "StartView");
            }},
        });
}

void SplashView::OnUpdate()
{
    bool bSkip = false;
    for (int i = 0; i < 4; ++i)
    {
        if (OGamePadJustPressed(OStartBtn, i) ||
            OGamePadJustPressed(OABtn, i))
        {
            bSkip = true;
            break;
        }
    }
    if (OJustPressed(OINPUT_MOUSEB1) || bSkip)
    {
        FindNode("logo")->GetScaleAnim().stop();
        SendCommand(seed::eAppCommand::SWITCH_VIEW, "StartView");
    }
}

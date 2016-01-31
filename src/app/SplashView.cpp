#include "seed/Node.h"
#include "SplashView.h"
#include "SpriteString.h"

void SplashView::OnShow()
{
    OGetSound("RitualSFX_Glitch_Sequence.wav");
    OGetSound("RitualSFX_Glitch01.wav");
    OGetSound("RitualSFX_Glitch02.wav");

    Load("../../assets/views/splashview.xml");

    auto pLogo = FindNode("logo");
    auto pNut = FindNode("nut");
    pNut->SetVisible(false);

    OPlaySound("RitualSFX_Glitch_Sequence.wav");

    auto scale = pLogo->GetScale();
    pLogo->GetScaleAnim().startKeyframed(
        Vector2(scale.x, 0.f),
        {
            OAnimWait(Vector2(scale.x, 0.f), .5f),
            {scale, .1f, OEaseIn},
            {scale, 1.f, OTeleport, [pNut]{pNut->SetVisible(true); OPlaySound("RitualSFX_Glitch01.wav"); }},

            {Vector2(scale.x * 1.2f, scale.y * .6f), .02f, OEaseOut},
            {scale, .02f, OEaseIn, [pNut]{pNut->SetVisible(false); }},

            {scale, .5f, OTeleport, [pNut]{pNut->SetVisible(true); OPlaySound("RitualSFX_Glitch02.wav"); }},

            {Vector2(scale.x * 1.1f, scale.y * .8f), .02f, OEaseOut},
            {scale, .02f, OEaseIn, [pNut]{pNut->SetVisible(false); }},

            {scale, .65f, OTeleport, [pNut]{pNut->SetVisible(true); OPlaySound("RitualSFX_Glitch02.wav"); }},

            {Vector2(scale.x * 1.1f, scale.y * .65f), .02f, OEaseOut},
            {scale, .02f, OEaseIn, [pNut]{pNut->SetVisible(false); }},

            {scale, .75f, OTeleport, [pNut]{pNut->SetVisible(true); OPlaySound("RitualSFX_Glitch01.wav"); }},

            {Vector2(scale.x * 1.2f, scale.y * .7f), .02f, OEaseOut},
            {scale, .02f, OEaseIn, [pNut]{pNut->SetVisible(false); }},

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

#include "seed/Node.h"
#include "SplashView.h"
#include "SpriteString.h"

void SplashView::OnShow()
{
    auto pText = CreateSpriteString("cartoon.fnt");
    pText->SetPosition(OScreenCenterf);
    pText->SetCaption("Splash Screen");
    AddNode(pText);

    // Start a timer then switch to main menu
    m_timer.start(5.f, [this]
    {
        SendCommand(seed::eAppCommand::SWITCH_VIEW, "StartView");
    });
}

void SplashView::OnUpdate()
{
    if (OJustPressed(OINPUT_MOUSEB1))
    {
        m_timer.stop();
        SendCommand(seed::eAppCommand::SWITCH_VIEW, "StartView");
    }
}

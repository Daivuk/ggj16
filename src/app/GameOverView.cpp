#include "Node.h"
#include "GameOverView.h"
#include "SpriteString.h"
#include "Video.h"
#include "Emitter.h"
#include "Globals.h"

void GameOverView::OnShow()
{
    Load("../../assets/views/gameover.xml");

    auto pVideo = ((seed::Video*)FindNode("video"));

    pVideo->SetVolume(0.f);
    pVideo->Play();

    ((seed::Emitter*)FindNode("fire1"))->Start();
    ((seed::Emitter*)FindNode("fire2"))->Start();
    if (g_daysSurvived == 1)
    {
        ((seed::SpriteString*)FindNode("days"))->SetCaption("You survived for ^991" + std::to_string(g_daysSurvived) + "^999 day...");
    }
    else
    {
        ((seed::SpriteString*)FindNode("days"))->SetCaption("You survived for ^991" + std::to_string(g_daysSurvived) + "^999 days...");
    }
}

void GameOverView::OnUpdate()
{
    if (OGamePadJustPressed(OStartBtn, 0) ||
        OGamePadJustPressed(OStartBtn, 1) ||
        OGamePadJustPressed(OStartBtn, 2) ||
        OGamePadJustPressed(OStartBtn, 3))
    {
        SendCommand(seed::eAppCommand::SWITCH_VIEW, "StartView");
    }
}

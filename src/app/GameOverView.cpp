#include "Node.h"
#include "GameOverView.h"
#include "SpriteString.h"
#include "Video.h"
#include "Emitter.h"

void GameOverView::OnShow()
{
    Load("../../assets/views/gameover.xml");
    ((seed::Video*)FindNode("video"))->Play();
    ((seed::Emitter*)FindNode("fire1"))->Start();
    ((seed::Emitter*)FindNode("fire2"))->Start();
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

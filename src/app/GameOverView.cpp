#include "Node.h"
#include "GameOverView.h"
#include "SpriteString.h"

void GameOverView::OnShow()
{
    auto pText = CreateSpriteString("cartoon.fnt");
    pText->SetPosition(OScreenCenterf);
    pText->SetCaption("GAME OVER");
    AddNode(pText);

    pText->GetScaleAnim().start(Vector2(5, 5), Vector2(1.0f, 1.0f), .5f, OEaseIn);
}

void GameOverView::OnUpdate()
{
    if (OGamePadJustPressed(OABtn,0)
        || OGamePadJustPressed(OABtn, 1)
        || OGamePadJustPressed(OABtn, 2)
        || OGamePadJustPressed(OABtn, 3)
       )
    {
        SendCommand(seed::eAppCommand::POP_VIEW, "");
        SendCommand(seed::eAppCommand::POP_VIEW, "");
        SendCommand(seed::eAppCommand::PUSH_VIEW, "StartView");
    }
}

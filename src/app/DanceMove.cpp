#include "DanceMove.h"
#include "View.h"
#include "Sprite.h"


DanceMove::DanceMove(const DanceMoveButtonVect& in_buttons)
{
    m_buttons = in_buttons;
}

void DanceMove::Show(float in_time, Entity* in_entity, seed::View* in_container)
{
    m_playerCompleted.clear();
    m_container = in_container;
    const float timeToDisplayMove = in_time;

    // show the move on top of the passed entity
    const float buttonWidth = 1.f;
    float nextX = (float)(m_buttons.size() - 1) * (-buttonWidth / 2.f);
    
    for (onut::GamePad::eGamePad button : m_buttons)
    {
        seed::Sprite* sprite = CreateSpriteForButton(button);
        m_displayedButtons.push_back(sprite);

        sprite->GetPositionAnim().startKeyframed(
        Vector2(0, 0),
        {
            { Vector2(nextX, -1.5f), .4f, OEaseOut },
            OAnimWait(Vector2(nextX, -1.5f), timeToDisplayMove - .4f),
            { Vector2(nextX, -2.5f), .4f, OLinear },
        });
        sprite->GetScaleAnim().start(Vector2(0, 0), Vector2(SPRITE_SCALE, SPRITE_SCALE) * .4f, .4f, OEaseOut);
        sprite->GetColorAnim().startKeyframed(
        Color(1.f, 1.f, 1.f, 0.f),
        {
            { Color(1.f, 1.f, 1.f, 1.f), .4f, OEaseOut },
            OAnimWait(Color(1.f, 1.f, 1.f, 1.f), timeToDisplayMove - .4f),
            { Color(1.f, 1.f, 1.f, 0.f), .4f, OLinear },
        });
        in_entity->Attach(sprite);

        nextX += buttonWidth;
    }
}

void DanceMove::Hide()
{
    for (seed::Sprite* s : m_displayedButtons)
    {
        //s->GetColorAnim().startFromCurrentKeyframed(
        //{ Color(1.f, 1.f, 1.f, 0.f), .3f, OLinear, HideAnimEnded }, ODontLoop);
        m_container->DeleteNode(s);
    }
    m_displayedButtons.clear();
}

bool DanceMove::IsNailed(DanceMoveButtonVect& in_playerInputs, int in_playerControllerIndex)
{
    int buttonIndex = 0;
    onut::GamePad::eGamePad lookingForButton = m_buttons[buttonIndex];

    for (size_t i = 0; i < in_playerInputs.size(); ++i)
    {
        if (IsSequenceFound(in_playerInputs, i))
        {
            m_playerCompleted.push_back(in_playerControllerIndex);
            return true;
        }
    }

    // if we are here it means the sequence was not completed
    return false;
}

bool DanceMove::IsSequenceFound(DanceMoveButtonVect& in_playerInputs, int startIndex)
{
    int buttonIndex = 0;
    onut::GamePad::eGamePad lookingForButton = m_buttons[buttonIndex];

    for (size_t i = startIndex; i < in_playerInputs.size(); ++i)
    {
        if (in_playerInputs[i] == lookingForButton)
        {
            ++buttonIndex;
            if (buttonIndex >= (int)m_buttons.size())
            {
                return true; // sequence is nailed
            }
            lookingForButton = m_buttons[buttonIndex];
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool DanceMove::PlayerNeedsToComplete(int in_playerControllerIndex)
{
    for (int& playerIndex : m_playerCompleted)
    {
        if (playerIndex == in_playerControllerIndex)
        {
            return false; // already completed for him
        }
    }
    return true;
}

seed::Sprite* DanceMove::CreateSpriteForButton(onut::GamePad::eGamePad in_button)
{
    seed::Sprite* result = nullptr;
    switch (in_button)
    {
        case OABtn:
        {
            result = m_container->CreateSprite("button_a.png");
            break;
        }
        case OBBtn:
        {
            result = m_container->CreateSprite("button_b.png");
            break;
        }
        case OXBtn:
        {
            result = m_container->CreateSprite("button_x.png");
            break;
        }
        case OYBtn:
        {
            result = m_container->CreateSprite("button_y.png");
            break;
        }
        case OLBBtn:
        {
            result = m_container->CreateSprite("button_lb.png");
            break;
        }
        case ORBBtn:
        {
            result = m_container->CreateSprite("button_rb.png");
            break;
        }
        case ORTBtn:
        {
            result = m_container->CreateSprite("button_rt.png");
            break;
        }
        case OLTBtn:
        {
            result = m_container->CreateSprite("button_lt.png");
            break;
        }
    }
    result->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    return result;
}

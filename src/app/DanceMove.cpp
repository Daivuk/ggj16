#include "DanceMove.h"
#include "View.h"
#include "Sprite.h"


DanceMove::DanceMove(const DanceMoveButtonVect& in_buttons)
{
    m_buttons = in_buttons;
}

void DanceMove::Show(float in_time, Entity* in_entity, seed::View* in_container)
{
    OPlaySoundCue("RitualCues_LetterAppear.cue");

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
        sprite->GetPositionAnim().start(Vector2(0, 0), Vector2(nextX, -1.5f), .4f, OEaseOut);


        //sprite->GetPositionAnim().startKeyframed(
        //Vector2(0, 0),
        //{
        //    { Vector2(nextX, -1.5f), .4f, OEaseOut },
        //    OAnimWait(Vector2(nextX, -1.5f), timeToDisplayMove - .4f),
        //    { Vector2(nextX, -2.5f), .4f, OLinear },
        //});
        sprite->GetScaleAnim().start(Vector2(0, 0), Vector2(SPRITE_SCALE, SPRITE_SCALE) * .8f, .4f, OEaseOut);
        sprite->GetColorAnim().start(Color(1.f, 1.f, 1.f, 0.f), Color(1.f, 1.f, 1.f, 1.f), .4f);
        //sprite->GetColorAnim().startKeyframed(
        //Color(1.f, 1.f, 1.f, 0.f),
        //{
        //    { Color(1.f, 1.f, 1.f, 1.f), .4f, OEaseOut },
        //    OAnimWait(Color(1.f, 1.f, 1.f, 1.f), timeToDisplayMove - .4f),
        //    { Color(1.f, 1.f, 1.f, 0.f), .4f, OLinear },
        //});
        in_entity->Attach(sprite);

        nextX += buttonWidth;
    }
}

void DanceMove::Hide()
{
    for (size_t i = 0; i < m_displayedButtons.size(); ++i )
    {
        seed::Sprite* s = m_displayedButtons[i];

        s->GetColorAnim().startFromCurrent(Color(1, 1, 1, 0), .5f);
        s->GetScaleAnim().startFromCurrent(s->GetScale() * 1.5f, .5f);
        s->GetPositionAnim().startFromCurrent(s->GetPosition() - Vector2(0,1), .5f);
    }
}

bool DanceMove::IsDoneHiding()
{
    for (size_t i = 0; i < m_displayedButtons.size(); ++i)
    {
        if (m_displayedButtons[i]->GetColorAnim().isPlaying())
        {
            return false;
        }
    }

    // if we're here it means we are done
    ClearButtons();
    return true;
}

void DanceMove::ClearButtons()
{
    for (size_t i = 0; i < m_displayedButtons.size(); ++i)
    {
        m_container->DeleteNode(m_displayedButtons[i]);
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
            result = m_container->CreateSprite("fire_button_a.png");
            break;
        }
        case OBBtn:
        {
            result = m_container->CreateSprite("fire_button_b.png");
            break;
        }
        case OXBtn:
        {
            result = m_container->CreateSprite("fire_button_x.png");
            break;
        }
        case OYBtn:
        {
            result = m_container->CreateSprite("fire_button_y.png");
            break;
        }
    }
    result->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    return result;
}

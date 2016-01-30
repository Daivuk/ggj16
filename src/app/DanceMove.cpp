#include "DanceMove.h"
#include "View.h"
#include "Sprite.h"


DanceMove::DanceMove(const DanceMoveButtonVect& in_buttons)
{
    m_buttons = in_buttons;
}

void DanceMove::Show(Entity* in_entity, seed::View* in_container)
{
    m_container = in_container;

    // show the move on top of the passed entity
    for (onut::GamePad::eGamePad button : m_buttons)
    {
        seed::Sprite* sprite = CreateSpriteForButton(button);
        m_displayedButtons.push_back(sprite);

        sprite->GetPositionAnim().start(Vector2(0, 0), Vector2(0, -1.5f), .4f, OEaseOut);
        sprite->GetScaleAnim().start(Vector2(0, 0), Vector2(SPRITE_SCALE, SPRITE_SCALE) * .4f, .4f, OEaseOut);
        sprite->GetColorAnim().start(Color(1.f, 1.f, 1.f, 0.f), Color(1.f, 1.f, 1.f, 1.f), 1.2f);
        in_entity->Attach(sprite);
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

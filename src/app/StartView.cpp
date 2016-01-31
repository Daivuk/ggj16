#include "StartView.h"
#include "Emitter.h"
#include "Sprite.h"
#include "Globals.h"

bool g_activePlayer[4];

StartView::StartView()
{

}

StartView::~StartView()
{

}

void StartView::OnShow()
{
    Load("../../assets/views/playerselectview.xml");
    ((seed::Emitter*)FindNode("fire1"))->Start();
    ((seed::Emitter*)FindNode("fire2"))->Start();

    CreateDude(FindNode("ped0"), 0);
    CreateDude(FindNode("ped1"), 1);
    CreateDude(FindNode("ped2"), 2);
    CreateDude(FindNode("ped3"), 3);
}

void StartView::OnHide()
{
}

void StartView::CreateDude(seed::Node* pParent, int id)
{
    m_dudes[id] = CreateSpriteWithSpriteAnim("guruAnims.spriteanim", "idle_down" + std::to_string(id));
    pParent->Attach(m_dudes[id]);
    m_dudes[id]->SetScale(Vector2(.75f));
    m_dudes[id]->SetPosition(Vector2(0, -3));
    m_dudes[id]->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    m_dudes[id]->SetColor(Color::Black);

    g_activePlayer[id] = false;
}

void StartView::OnUpdate()
{
    for (int i = 0; i < 4; ++i)
    {
        auto pGamePad = OGamePad(i);
        if (!pGamePad->isConnected())
        {
            g_activePlayer[i] = false;
        }
        else if (OGamePadJustPressed(OABtn, i))
        {
            g_activePlayer[i] = !g_activePlayer[i];
            if (g_activePlayer[i])
            {
                OPlaySoundCue("RitualCues_ButtonPress.cue");
                m_dudes[i]->GetPositionAnim().startKeyframed(
                    Vector2(0, -3),
                    {
                        {Vector2(0, -3), 0.f, OTeleport}, {Vector2(0, -10), .15f, OEaseOut}, {Vector2(0, -3), .3f, OBounceOut}
                    });
                m_dudes[i]->SetSpriteAnim("dance" + std::to_string(i) + std::to_string(i));
            }
            else
            {
                m_dudes[i]->SetSpriteAnim("idle_down" + std::to_string(i));
            }
        }
        if (g_activePlayer[i])
        {
            m_dudes[i]->SetColor(Color::White);
        }
        else
        {
            m_dudes[i]->SetColor(Color::Black);
        }
    }

    if (g_activePlayer[0] && OGamePadJustPressed(OStartBtn, 0))
    {
        SendCommand(seed::eAppCommand::SWITCH_VIEW, "GameView");
    }
}

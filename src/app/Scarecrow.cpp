#include "Scarecrow.h"
#include "View.h"
#include "Globals.h"
#include "Sprite.h"
#include "GameView.h"
#include "Emitter.h"

Scarecrow::Scarecrow(seed::View* pView)
    : m_pView(pView)
{
    m_health = 200;

    m_pSprite = pView->CreateSprite("scarecrow.png");
    m_pSprite->SetScale(Vector2(SPRITE_SCALE));
    m_pSprite->SetAlign(Vector2(.5f, .75f));
    m_pSprite->SetFilter(onut::SpriteBatch::eFiltering::Nearest);
    Attach(m_pSprite);

    m_pEmitters[0] = pView->CreateEmitter("ScarecrowFireFX.pex");
    m_pEmitters[0]->SetEmitWorld(false);
    m_pEmitters[0]->SetPosition(Vector2(0, -10));
    m_pEmitters[0]->SetScale(Vector2(.25f));
    m_pSprite->Attach(m_pEmitters[0]);
}

void Scarecrow::Place(const Vector2& position)
{
    SetPosition(position);
    g_gameView->AddEntity(this);

    m_pView->CreateCirclePhysicsForNode(this, .15f, true);

    lightEnabled = true;
    lightRadius = 2.5f;
    lightColor = Color(1, .75f, .25f, 1);

    m_pEmitters[0]->Start();
}

bool Scarecrow::InflictDamage(float in_damage)
{
    float strengh = .5f;
    float timeScale = .75f;
    m_pSprite->GetAngleAnim().startKeyframed(0.f,
    {
        {-30.f * strengh, .1f * timeScale, OEaseOut},
        {30.f * strengh, .1f * timeScale, OEaseBoth},
        {-15.f * strengh, .05f * timeScale, OEaseBoth},
        {15.f * strengh, .05f * timeScale, OEaseBoth},
        {-7.5f * strengh, .025f * timeScale, OEaseBoth},
        {7.5f * strengh, .025f * timeScale, OEaseBoth},
        {-3.75f * strengh, .0125f * timeScale, OEaseBoth},
        {0.f, .0125f, OEaseIn},
    });
    return Entity::InflictDamage(in_damage);
}

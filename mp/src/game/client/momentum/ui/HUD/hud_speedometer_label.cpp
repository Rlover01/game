#include "cbase.h"

#include "hud_speedometer_label.h"

#include <vgui_controls/AnimationController.h>
#include "iclientmode.h"
#include "momentum/util/mom_util.h"
#include "mom_player_shared.h"

#include "tier0/memdbgon.h"

// 1 unit = 19.05mm -> 0.01905m -> 0.00001905Km(/s) -> 0.06858Km(/h)
#define UPS_TO_KMH_FACTOR 0.06858f
// 1 unit = 0.75", 1 mile = 63360. 0.75 / 63360 ~~> 0.00001184"(/s) ~~> 0.04262MPH
#define UPS_TO_MPH_FACTOR 0.04262f

using namespace vgui;

extern ConVar sv_gravity;

SpeedometerLabel::SpeedometerLabel(Panel *parent, const char *panelName)
    : Label(parent, panelName, ""), m_bSupportsEnergyUnits(false),
      m_eUnitType(SPEEDOMETER_UNITS_UPS), m_pflAlpha(nullptr)
{
    Reset();
}

void SpeedometerLabel::SetVisible(bool bVisible)
{
    BaseClass::SetVisible(bVisible);
    // parent's layout depends on the visiblity of this, so invalidate it
    GetParent()->InvalidateLayout();
}

void SpeedometerLabel::OnThink()
{
    BaseClass::OnThink();

    if (!HasFadeOutAnimation())
        return;

    SetAlpha(*m_pflAlpha);
}

void SpeedometerLabel::Reset()
{
    m_flCurrentValue = 0.0f;
    m_flPastValue = 0.0f;
    m_flDiff = 0.0f;
    BaseClass::SetText("");

    SetAlpha(HasFadeOutAnimation() ? 0 : 255);
}

void SpeedometerLabel::SetText(int value)
{
    char szValue[BUFSIZELOCL];
    Q_snprintf(szValue, sizeof(szValue), "%i", value);
    BaseClass::SetText(szValue);
}

void SpeedometerLabel::Update(float value)
{
    m_flCurrentValue = value;

    ConvertUnits();
    SetText(m_flCurrentValue);
    if (HasFadeOutAnimation())
        StartFadeout();

    m_flPastValue = m_flCurrentValue;
}

void SpeedometerLabel::SetUnitType(int type) 
{ 
    if (type >= SPEEDOMETER_UNITS_FIRST && type <= SPEEDOMETER_UNITS_LAST)
        SetUnitType(SpeedometerUnits_t(type));
}

void SpeedometerLabel::ConvertUnits()
{
    if (m_eUnitType == SPEEDOMETER_UNITS_UPS)
        return;

    switch (m_eUnitType)
    {
    case SPEEDOMETER_UNITS_KMH:
        m_flCurrentValue *= UPS_TO_KMH_FACTOR;
        m_flDiff *= UPS_TO_KMH_FACTOR;
        break;
    case SPEEDOMETER_UNITS_MPH:
        m_flCurrentValue *= UPS_TO_MPH_FACTOR;
        m_flDiff *= UPS_TO_MPH_FACTOR;
        break;
    case SPEEDOMETER_UNITS_ENERGY:
        // outlier as it requires more than just 1 velocity
        if (m_bSupportsEnergyUnits)
        {
            const auto gravity = sv_gravity.GetFloat();
            const auto pPlayer = C_MomentumPlayer::GetLocalMomPlayer();
            Vector velocity = pPlayer->GetAbsVelocity();
            const auto pEntData = pPlayer->GetCurrentUIEntData();
            if (!pEntData)
                break;

            m_flCurrentValue =
                (velocity.LengthSqr() / 2.0f + gravity * (pPlayer->GetLocalOrigin().z - pEntData->m_flLastJumpZPos)) /
                gravity;
        }
        break;
    default:
        break;
    }
}

void SpeedometerLabel::SetFadeOutAnimation(char *animationName, float *animationAlpha)
{
    Q_strncpy(m_pszAnimationName, animationName, sizeof(m_pszAnimationName));
    m_pflAlpha = animationAlpha;
}

bool SpeedometerLabel::StartFadeout()
{
    Assert(HasFadeOutAnimation());
    g_pClientMode->GetViewportAnimationController()->StopAnimationSequence(GetParent(), m_pszAnimationName);
    *m_pflAlpha = 255.0f;
    return g_pClientMode->GetViewportAnimationController()->StartAnimationSequence(m_pszAnimationName);
}
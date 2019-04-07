#include "AbilitySystem/RTSGameplayEffect.h"


UTexture2D* URTSGameplayEffect::GetIcon()
{
    return Icon;
}

FText URTSGameplayEffect::GetEffectName()
{
    return !EffectName.IsEmpty() ? EffectName : FText::FromString(GetClass()->GetName());
}

FText URTSGameplayEffect::GetEffectDescription()
{
    return EffectDescription;
}

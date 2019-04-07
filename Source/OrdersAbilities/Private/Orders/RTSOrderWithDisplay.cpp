#include "Orders/RTSOrderWithDisplay.h"

#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"


UTexture2D* URTSOrderWithDisplay::GetNormalIcon(const AActor* OrderedActor, int32 Index) const
{
    return NormalIcon;
}

UTexture2D* URTSOrderWithDisplay::GetHoveredIcon(const AActor* OrderedActor, int32 Index) const
{
    return HoveredIcon;
}

UTexture2D* URTSOrderWithDisplay::GetPressedIcon(const AActor* OrderedActor, int32 Index) const
{
    return PressedIcon;
}

UTexture2D* URTSOrderWithDisplay::GetDisabledIcon(const AActor* OrderedActor, int32 Index) const
{
    return DisabledIcon;
}

FText URTSOrderWithDisplay::GetName(const AActor* OrderedActor, int32 Index) const
{
    return Name;
}

FText URTSOrderWithDisplay::GetDescription(const AActor* OrderedActor, int32 Index) const
{
    return Description;
}

int32 URTSOrderWithDisplay::GetOrderButtonIndex() const
{
    return OrderButtonIndex;
}

bool URTSOrderWithDisplay::HasFixedOrderButtonIndex() const
{
    return bUseFixedOrderButtonIndex;
}

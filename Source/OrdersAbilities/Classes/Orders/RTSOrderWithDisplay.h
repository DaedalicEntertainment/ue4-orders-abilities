#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSOrder.h"
#include "Text.h"
#include "RTSOrderWithDisplay.generated.h"

class AActor;
class UTexture2D;

/**
 * An order with a fixed icon, name and description text that is independent from the index.
 */
UCLASS(BlueprintType, Abstract, Blueprintable)
class ORDERSABILITIES_API URTSOrderWithDisplay : public URTSOrder
{
    GENERATED_BODY()

public:
    //~ Begin URTSOrder Interface
    virtual UTexture2D* GetNormalIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetHoveredIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetPressedIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetDisabledIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual FText GetName(const AActor* OrderedActor, int32 Index) const override;
    virtual FText GetDescription(const AActor* OrderedActor, int32 Index) const override;
    virtual int32 GetOrderButtonIndex() const override;
    virtual bool HasFixedOrderButtonIndex() const override;
    //~ End URTSOrder Interface

private:
    /** Normal icon of the order. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UTexture2D* NormalIcon;

    /** Hovered icon of the order. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UTexture2D* HoveredIcon;

    /** Pressed icon of the order. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UTexture2D* PressedIcon;

    /** Disabled icon of the order. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UTexture2D* DisabledIcon;

    /** Name of the order. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FText Name;

    /** Description of the order. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FText Description;

    /** Whether to use a fixed index of the button when shown in the UI, instead of just lining it up among others. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bUseFixedOrderButtonIndex;

    /** Index of the button when shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    int32 OrderButtonIndex;
};

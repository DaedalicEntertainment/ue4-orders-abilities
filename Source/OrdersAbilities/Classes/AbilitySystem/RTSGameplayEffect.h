#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RTSGameplayEffect.generated.h"

/** Gameplay effect with RTS features, such as an UI icon. */
UCLASS(BlueprintType, Abstract, Blueprintable, HideCategories = (Advanced, Input))
class ORDERSABILITIES_API URTSGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()

public:
    /** Gets the icon of this effect. Can be shown in the UI. */
    UFUNCTION(Category = RTS, BlueprintPure)
    UTexture2D* GetIcon();

    /** Gets the name of this effect. Can be shown in the UI. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FText GetEffectName();

    /** Gets the description of this effect. Can be shown in the UI. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FText GetEffectDescription();

private:
    /** Icon of this ability. Can be shown in the UI. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UTexture2D* Icon;

    /** Name of this effect. Can be shown in the UI. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FText EffectName;

    /** Description of this effect. Can be shown in the UI. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FText EffectDescription;
};

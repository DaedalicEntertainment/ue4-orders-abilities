#pragma once

#include "CoreMinimal.h"
#include "Text.h"
#include "Orders/RTSOrderWithBehavior.h"
#include "Orders/RTSTargetType.h"
#include "RTSUseAbilityOrder.generated.h"

class AActor;
struct FRTSOrderTargetData;
class URTSAbilitySystemComponent;
class UGameplayAbility;

/**
 * Orders the actor to use a ability.
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable, hideCategories = "RTSDisplay")
class ORDERSABILITIES_API URTSUseAbilityOrder : public URTSOrderWithBehavior
{
    GENERATED_BODY()

public:
    URTSUseAbilityOrder();

    //~ Begin URTSOrder Interface
    virtual bool CanObeyOrder(const AActor* OrderedActor, int32 Index,
                              FRTSOrderErrorTags* OutErrorTags = nullptr) const override;
    virtual ERTSTargetType GetTargetType(const AActor* OrderedActor, int32 Index) const override;
    virtual void IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
                            FRTSOrderCallback Callback, const FVector& HomeLocation) const override;
    virtual UTexture2D* GetNormalIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetHoveredIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetPressedIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetDisabledIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual FText GetName(const AActor* OrderedActor, int32 Index) const override;
    virtual FText GetDescription(const AActor* OrderedActor, int32 Index) const override;
    virtual void GetTagRequirements(const AActor* OrderedActor, int32 Index,
                                    FRTSOrderTagRequirements& OutTagRequirements) const override;

    virtual float GetRequiredRange(const AActor* OrderedActor, int32 Index) const override;
    virtual FRTSOrderPreviewData GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const override;
    virtual ERTSOrderProcessPolicy GetOrderProcessPolicy(const AActor* OrderedActor, int32 Index) const override;
    virtual ERTSOrderGroupExecutionType GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const override;
    virtual bool IsHumanPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const override;
    virtual bool GetHumanPlayerAutoOrderInitialState(const AActor* OrderedActor, int32 Index) const override;
    virtual bool IsAIPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const override;
    virtual bool GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index,
                                              float& OutAcquisitionRadius) const override;
    virtual float GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const;
    //~ End URTSOrder Interface

protected:
    virtual UGameplayAbility* GetAbility(const URTSAbilitySystemComponent* AbilitySystem, int32 Index) const;

private:
    UTexture2D* GetIcon(const AActor* OrderedActor, int32 Index) const;
};

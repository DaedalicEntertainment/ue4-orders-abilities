#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSOrderWithDisplay.h"
#include "RTSOrderWithBehavior.generated.h"

class AActor;
class UBehaviorTree;

/**
 * Order with custom behavior in form of a behavior tree.
 */
UCLASS(BlueprintType, Abstract, Blueprintable)
class ORDERSABILITIES_API URTSOrderWithBehavior : public URTSOrderWithDisplay
{
    GENERATED_BODY()

public:
    URTSOrderWithBehavior();

    /** Gets the behavior tree that should run in order to obey this order. */
    UBehaviorTree* GetBehaviorTree() const;

    /** Whether to restart the behavior tree whenever a new order of this type is issued. */
    bool ShouldRestartBehaviourTree() const;

    //~ Begin URTSOrder Interface
    virtual void IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
                            FRTSOrderCallback Callback, const FVector& HomeLocation) const override;
    virtual bool GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index,
                                              float& OutAcquisitionRadius) const override;
    //~ End URTSOrder Interface

private:
    /** The behavior tree that should run in order to obey this order. */
    UPROPERTY(Category = "RTS Behavior", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UBehaviorTree* BehaviorTree;

    /** Whether to restart the behavior tree whenever a new order of this type is issued. */
    UPROPERTY(Category = "RTS Behavior", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bShouldRestartBehaviourTree;

    /** The specific acquisition radius for this order. */
    UPROPERTY(Category = "RTS Behavior", EditDefaultsOnly, BlueprintReadOnly,
              meta = (AllowPrivateAccess = true, EditCondition = bIsAcquisitionRadiusOverridden))
    float AcquisitionRadiusOverride;

    /** Whether this order uses a specific acquisition radius. */
    UPROPERTY(Category = "RTS Behavior", EditDefaultsOnly, BlueprintReadOnly,
              meta = (AllowPrivateAccess = true, InlineEditConditionToggle = true))
    bool bIsAcquisitionRadiusOverridden;
};

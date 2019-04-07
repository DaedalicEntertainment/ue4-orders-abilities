#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSCharacterAIOrder.h"
#include "RTSAttackOrder.generated.h"

class AActor;
struct FRTSOrderTargetData;

/**
 * Orders the actor to attack a unit or building.
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable)
class ORDERSABILITIES_API URTSAttackOrder : public URTSCharacterAIOrder
{
    GENERATED_BODY()

public:
    URTSAttackOrder();

    //~ Begin URTSOrder Interface
    virtual void CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
                                                 const FRTSOrderTargetData& TargetData,
                                                 TArray<FVector2D>& OutTargetLocations) const override;
    virtual float GetRequiredRange(const AActor* OrderedActor, int32 Index) const override;

    virtual float GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData,
                                 int32 Index) const override;
    virtual bool AreAutoOrdersAllowedDuringOrder() const override;
    //~ End URTSOrder Interface
};

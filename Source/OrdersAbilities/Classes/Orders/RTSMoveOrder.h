#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSCharacterAIOrder.h"
#include "RTSMoveOrder.generated.h"

class AActor;
struct FRTSOrderTargetData;

/**
 * Standard unit movement to a fixed target location on the map. Calculates a formation for multiple units using their
 * formation rank.
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable)
class ORDERSABILITIES_API URTSMoveOrder : public URTSCharacterAIOrder
{
    GENERATED_BODY()

public:
    URTSMoveOrder();

    //~ Begin URTSOrder Interface
    virtual void CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
                                                 const FRTSOrderTargetData& TargetData,
                                                 TArray<FVector2D>& OutTargetLocations) const override;
    //~ End URTSOrder Interface

private:
    /**
     * Creates a block formation of unit locations.
     * @param UnitCount The required amount of locations.
     * @param Direction The direction the formation should face to. Does not have to be normalized.
     * @param TargetLocation Target location of the formation.
     * @param[out] An array that will be filled with the locations starting from the front line.
     */
    void CalculateFormation(int32 UnitCount, const FVector2D Direction, const FVector2D TargetLocation,
                            TArray<FVector2D>& OutLocations) const;

    /** Gets the 2D center of the specified group of actors. */
    FVector2D GetCenterOfGroup(const TArray<AActor*>& Actors) const;

    /** Gets the formation rank of the specified unit. */
    int32 GetFormationRank(const AActor* Unit) const;
};

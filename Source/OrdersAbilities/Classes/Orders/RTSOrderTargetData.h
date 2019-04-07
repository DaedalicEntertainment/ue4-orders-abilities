#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RTSOrderTargetData.generated.h"

class AActor;

/** Targeting data for an order. */
USTRUCT(BlueprintType)
struct ORDERSABILITIES_API FRTSOrderTargetData
{
    GENERATED_BODY()

    FRTSOrderTargetData();
    FRTSOrderTargetData(AActor* InActor, const FVector2D InLocation, const FGameplayTagContainer& InTargetTags);

    /** The target actor. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadWrite)
    AActor* Actor;

    /** The target location. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadWrite)
    FVector2D Location;

    /** The target tags. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadWrite)
    FGameplayTagContainer TargetTags;

    /**
     * Get a textual representation of this order target data.
     * @return A string describing the order target data.
     */
    FString ToString() const;
};

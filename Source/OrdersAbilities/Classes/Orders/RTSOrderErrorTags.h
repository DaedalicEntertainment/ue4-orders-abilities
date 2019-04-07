#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSOrder.h"
#include "GameplayTagContainer.h"
#include "RTSOrderErrorTags.generated.h"

class AActor;

/**
 * Encloses collections of missing, blocking and error tags that describe why an order cannot be issued.
 */
USTRUCT(BlueprintType)
struct ORDERSABILITIES_API FRTSOrderErrorTags
{
    GENERATED_BODY()

    /** List of required tags that are missing. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer MissingTags;

    /** List of tags that are blocking the execution of the order. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer BlockingTags;

    /** List of additional error tags that describe additional missing requirements for the execution of the order. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer ErrorTags;

    /** Whether any tags have been specified, or not. */
    bool IsEmpty() const;

    /**
     * Get a textual representation of this order.
     * @return A string describing the order.
     */
    FString ToString() const;
};
#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSOrder.h"
#include "RTSOrderTypeWithIndex.generated.h"

class AActor;

/**
 * An order type with an index.
 */
USTRUCT(BlueprintType)
struct ORDERSABILITIES_API FRTSOrderTypeWithIndex
{
    GENERATED_BODY()

    FRTSOrderTypeWithIndex();
    FRTSOrderTypeWithIndex(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex = -1);

    /** Type of this order. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<URTSOrder> OrderType;

    /** Additional index. This is needed for certain orders to differentiate. Default '-1'. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    int32 Index;

    /**
     * Get a textual representation of this order.
     * @return A string describing the order.
     */
    FString ToString() const;

    bool operator==(const FRTSOrderTypeWithIndex& Other) const;
    bool operator!=(const FRTSOrderTypeWithIndex& Other) const;
};
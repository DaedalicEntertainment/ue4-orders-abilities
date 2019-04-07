#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSOrder.h"
#include "RTSOrderData.generated.h"

class AActor;

/**
 * An order that can be issued to units and buildings.
 */
USTRUCT(BlueprintType)
struct ORDERSABILITIES_API FRTSOrderData
{
    GENERATED_BODY()

    FRTSOrderData();
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, AActor* InTarget);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, FVector2D InLocation);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, AActor* InTarget, FVector2D InLocation);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, AActor* InTarget);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, FVector2D InLocation);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, AActor* InTarget, FVector2D InLocation);

    /** Type of this order. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<URTSOrder> OrderType;

    /** Indicates that 'Location' is valid and should be used by this order. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle = true))
    uint8 bUseLocation : 1;

    /**
     * Target location of the order. Use 'bUseLocation' to check if this location is really set and should be used.
     */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bUseLocation))
    FVector2D Location;

    /** Target actor for this order. 'Null' if no order has been specified. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    AActor* Target;

    /** Additional index. This is needed for certain orders to differentiate. Default '-1'. */
    UPROPERTY(Category = RTS, EditAnywhere, BlueprintReadWrite)
    int32 Index;

    /**
     * Get a textual representation of this order.
     * @return A string describing the order.
     */
    FString ToString() const;

    bool operator==(const FRTSOrderData& Other) const;

    bool operator!=(const FRTSOrderData& Other) const;
};
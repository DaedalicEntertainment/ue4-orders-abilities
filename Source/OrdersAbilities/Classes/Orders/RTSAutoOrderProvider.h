#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Orders/RTSOrderData.h"
#include "Orders/RTSOrderTypeWithIndex.h"
#include "RTSAutoOrderProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URTSAutoOrderProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * Implement this interface on an 'UActorComponent' to provide auto orders that should be issued to this unit
 * automatically.
 */
class ORDERSABILITIES_API IRTSAutoOrderProvider
{
    GENERATED_BODY()

public:
    /**
     * Should fill the passed array with orders that should be issued to this unit automatically.
     */
    UFUNCTION(Category = RTS, BlueprintNativeEvent, BlueprintCallable)
    void GetAutoOrders(TArray<FRTSOrderTypeWithIndex>& OutAutoOrders);
};

#pragma once

#include "CoreMinimal.h"
#include "Orders/RTSCharacterAIOrder.h"
#include "RTSStopOrder.generated.h"

class AActor;

/**
 * Stops the actor and switches it in the idle state.
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable)
class ORDERSABILITIES_API URTSStopOrder : public URTSCharacterAIOrder
{
    GENERATED_BODY()
public:
    URTSStopOrder();

    //~ Begin URTSOrder Interface
    virtual bool AreAutoOrdersAllowedDuringOrder() const;
    //~ End URTSOrder Interface
};

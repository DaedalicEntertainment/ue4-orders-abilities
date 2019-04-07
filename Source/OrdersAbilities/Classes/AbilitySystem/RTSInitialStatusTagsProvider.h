#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "RTSInitialStatusTagsProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URTSInitialStatusTagsProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * Implement this interface on an 'UActorComponent' to provide initial status tags for the unit.
 */
class ORDERSABILITIES_API IRTSInitialStatusTagsProvider
{
    GENERATED_BODY()

public:
    /**
     * Should fill the passed tag container with permanent status tags.
     */
    UFUNCTION(Category = RTS, BlueprintNativeEvent, BlueprintCallable)
    void GetInitialStatusTags(FGameplayTagContainer& OutTagContainer);
};

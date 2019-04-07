#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "RTSAttributeSet.generated.h"

/**
 * RTS base class for ability system attribute sets
 */
UCLASS(BlueprintType, Abstract)
class ORDERSABILITIES_API URTSAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    /** Always called after the attribute values where initialized using a curve table. */
    virtual void PostInitializeProperties(bool bInitialInit);

    /** Gets the status tags this attribute set grants to its owner. */
    virtual void GetDefaultStatusTags(FGameplayTagContainer& OutStatusTags) const;
};

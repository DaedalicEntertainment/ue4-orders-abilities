#pragma once

#include "CoreMinimal.h"

/**
 * The possible target types of an order.
 */
UENUM(BlueprintType)
enum class ERTSTargetType : uint8
{
    /** No specific target (AOE, Aura or automatically applied to self). */
    NONE,

    /** Order needs an actor as target. */
    ACTOR,

    /** Order needs vector location as target. */
    LOCATION,

    /** Order needs vector location that is used together with the unit location as a direction. */
    DIRECTION,

    /** Can't be activated. */
    PASSIVE
};

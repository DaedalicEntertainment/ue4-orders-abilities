#pragma once

UENUM(BlueprintType)
enum class ERTSOrderResult : uint8
{
    /** The order was fulfilled. */
    SUCCEEDED,

    /** The order was canceled. */
    CANCELED,

    /** The order has failed. */
    FAILED,
};
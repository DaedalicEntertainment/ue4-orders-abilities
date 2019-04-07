#pragma once

/**
 * Describes how an order is executed. This might determine how the order is displayed in the UI and it determines
 * how the order is handled by the order system.
 */
UENUM(BlueprintType)
enum class ERTSOrderProcessPolicy : uint8
{
    /**
     * An instant order is issued directly without changing the current order (ProductionOrder, SetRallyPointOrder,
     * Some Abilities etc.).
     */
    INSTANT,

    /**
     * The order is considered to have a duration (MoveOrder). The order will be canceled actively when another order is
     * issued.
     */
    CAN_BE_CANCELED,

    /**
     * The order is considered to have a duration (MoveOrder). The order will not be canceled actively when another
     * order is issued. It will only be canceled when the tag requirements for the order or its target are not full
     * filled anymore.
     */
    CAN_NOT_BE_CANCELED,
};
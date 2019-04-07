#pragma once

/**
 * To how many and which of the selected units should an order be issued to.
 */
UENUM(BlueprintType)
enum class ERTSOrderGroupExecutionType : uint8
{
    /** The order is simply issued to all selected units. */
    ALL,

    /** The order is issued to the currently selected subgroup. */
    SELECTED_SUB_GROUP,

    /** The order is only issued to the main selected unit. */
    SELECTED_UNIT,

    /**
     * The order is issued to a single unit that is capable to obey the order.
     * Note that if the order has a target, the highest value returned by RTSOrderHelperGetOrderTargetScore determines
     * the unit.
     */
    MOST_SUITABLE_UNIT,
};

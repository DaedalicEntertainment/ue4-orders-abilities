#include "Orders/RTSStopOrder.h"

#include "GameFramework/Actor.h"

#include "AbilitySystem/RTSGlobalTags.h"


URTSStopOrder::URTSStopOrder()
{
    TargetType = ERTSTargetType::NONE;
    bIsCreatingIndividualTargetLocations = false;

    TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Constructing());
}

bool URTSStopOrder::AreAutoOrdersAllowedDuringOrder() const
{
    return true;
}

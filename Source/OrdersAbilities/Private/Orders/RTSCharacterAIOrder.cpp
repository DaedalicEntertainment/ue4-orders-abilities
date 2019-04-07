#include "Orders/RTSCharacterAIOrder.h"

#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Actor.h"


URTSCharacterAIOrder::URTSCharacterAIOrder()
{
    TargetType = ERTSTargetType::NONE;
    bIsCreatingIndividualTargetLocations = false;

    GroupExecutionType = ERTSOrderGroupExecutionType::ALL;
}

ERTSTargetType URTSCharacterAIOrder::GetTargetType(const AActor* OrderedActor, int32 Index) const
{
    return TargetType;
}

bool URTSCharacterAIOrder::IsCreatingIndividualTargetLocations(const AActor* OrderedActor, int32 Index) const
{
    return bIsCreatingIndividualTargetLocations;
}

ERTSOrderGroupExecutionType URTSCharacterAIOrder::GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const
{
    return GroupExecutionType;
}

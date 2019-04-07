#include "Orders/RTSAttackOrder.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Actor.h"

#include "AbilitySystem/RTSAbilitySystemHelper.h"
#include "AbilitySystem/RTSAttackAttributeSet.h"
#include "AbilitySystem/RTSGlobalTags.h"
#include "Orders/RTSOrderTargetData.h"


URTSAttackOrder::URTSAttackOrder()
{
    TargetType = ERTSTargetType::ACTOR;
    bIsCreatingIndividualTargetLocations = true;

    TagRequirements.SourceRequiredTags.AddTag(URTSGlobalTags::Status_Permanent_CanAttack());
    TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Unarmed());
    TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Constructing());

    TagRequirements.TargetRequiredTags.AddTag(URTSGlobalTags::Status_Changing_IsAlive());
    TagRequirements.TargetRequiredTags.AddTag(URTSGlobalTags::Relationship_Visible());
    TagRequirements.TargetBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Invulnerable());
    TagRequirements.TargetBlockedTags.AddTag(URTSGlobalTags::Relationship_Friendly());

    SuccessTagRequirements.TargetBlockedTags.AddTag(URTSGlobalTags::Status_Changing_IsAlive());
}

void URTSAttackOrder::CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
                                                      const FRTSOrderTargetData& TargetData,
                                                      TArray<FVector2D>& OutTargetLocations) const
{
    // TODO: Calculate valid positions around building.
    Super::CreateIndividualTargetLocations(OrderedActors, TargetData, OutTargetLocations);
}

float URTSAttackOrder::GetRequiredRange(const AActor* OrderedActor, int32 Index) const
{
    return URTSAbilitySystemHelper::GetAttributeValue(OrderedActor, URTSAttackAttributeSet::RangeAttribute());
}

float URTSAttackOrder::GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData,
                                      int32 Index) const
{
    float Score = Super::GetTargetScore(OrderedActor, TargetData, Index);

    // Workers have a minor priority.
    if (TargetData.TargetTags.HasTag(URTSGlobalTags::Status_Permanent_CanGather()))
    {
        Score *= 0.7f;
    }

    // Buildings have a minor priority.
    if (TargetData.TargetTags.HasTag(URTSGlobalTags::Building()))
    {
        Score *= 0.5f;
    }

    return Score;
}

bool URTSAttackOrder::AreAutoOrdersAllowedDuringOrder() const
{
    return true;
}

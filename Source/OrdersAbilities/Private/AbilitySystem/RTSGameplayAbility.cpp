#include "AbilitySystem/RTSGameplayAbility.h"


URTSGameplayAbility::URTSGameplayAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // TODO: Is their any reason a RTS game play ability should always be server only?
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

    // This setting should always be false for an RTS. All abilities are fired or canceled on the server.
    bServerRespectsRemoteAbilityCancellation = false;

    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;

    AbilityProcessPolicy = ERTSAbilityProcessPolicy::INSTANT;

    GroupExecutionType = ERTSOrderGroupExecutionType::MOST_SUITABLE_UNIT;

    AcquisitionRadiusOverride = 0.0f;
    bIsAcquisitionRadiusOverridden = false;

    bHumanPlayerAutoAbility = false;
    bHumanPlayerAutoAutoAbilityInitialState = false;
    bAIPlayerAutoAbility = true;
}

ERTSTargetType URTSGameplayAbility::GetTargetType() const
{
    return TargetType;
}

ERTSOrderGroupExecutionType URTSGameplayAbility::GetGroupExecutionType() const
{
    return GroupExecutionType;
}

FGameplayTag URTSGameplayAbility::GetEventTriggerTag() const
{
    for (const FAbilityTriggerData& AbilityTrigger : AbilityTriggers)
    {
        if (AbilityTrigger.TriggerSource == EGameplayAbilityTriggerSource::GameplayEvent)
        {
            return AbilityTrigger.TriggerTag;
        }
    }

    return FGameplayTag();
}

const TArray<FAbilityTriggerData>& URTSGameplayAbility::GetAbilityTriggerData() const
{
    return AbilityTriggers;
}

UTexture2D* URTSGameplayAbility::GetIcon() const
{
    return Icon;
}

FText URTSGameplayAbility::GetName() const
{
    return Name;
}

FText URTSGameplayAbility::GetDescription(const AActor* Actor) const
{
    FText FormattedDescription;
    FormatDescription(Description, Actor, FormattedDescription);
    return FormattedDescription;
}

int32 URTSGameplayAbility::GetMaxLevel() const
{
    return MaxLevel;
}

FRTSOrderPreviewData URTSGameplayAbility::GetAbilityPreviewData() const
{
    return AbilityPreviewData;
}

void URTSGameplayAbility::FormatDescription_Implementation(const FText& InDescription, const AActor* Actor,
                                                           FText& OutDescription) const

{
    OutDescription = InDescription;
}

bool URTSGameplayAbility::ShouldShowAsOrderInUI()
{
    return bShouldShowAsOrderInUI;
}

bool URTSGameplayAbility::GetAcquisitionRadiusOverride(float& OutAcquisitionRadius) const
{
    OutAcquisitionRadius = AcquisitionRadiusOverride;
    return bIsAcquisitionRadiusOverridden;
}

bool URTSGameplayAbility::IsHumanPlayerAutoAbility() const
{
    return bHumanPlayerAutoAbility;
}

bool URTSGameplayAbility::GetHumanPlayerAutoAutoAbilityInitialState() const
{
    return bHumanPlayerAutoAutoAbilityInitialState;
}

bool URTSGameplayAbility::IsAIPlayerAutoAbility() const
{
    return bAIPlayerAutoAbility;
}

bool URTSGameplayAbility::IsTargetScoreOverriden() const
{
    return bIsTargetScoreOverridden;
}

bool URTSGameplayAbility::AreAbilityTasksActive() const
{
    return ActiveTasks.Num() > 0;
}

bool URTSGameplayAbility::ShouldActivateAbility(ENetRole Role) const
{
    // This is currently only used by CanActivateAbility to block clients from activating abilities themselves.
    // This in turn is also prevented by NetExecutionPolicy in our case.
    // However, CanActivateAbility is also used by clients in order to check the respective ability before issuing the
    // order.
    return true;
}

void URTSGameplayAbility::OnGameplayTaskActivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskActivated(Task);
}

void URTSGameplayAbility::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskDeactivated(Task);
}

void URTSGameplayAbility::OnAbilityLevelChanged_Implementation(int32 NewLevel)
{
}

void URTSGameplayAbility::GetTargetScore_Implementation(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, float& OutScore) const
{
    OutScore = 1.0f;
}

float URTSGameplayAbility::GetRange(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    return GetRange();
}

ERTSAbilityProcessPolicy URTSGameplayAbility::GetAbilityProcessPolicy() const
{
    return AbilityProcessPolicy;
}

FGameplayTagContainer URTSGameplayAbility::GetActivationRequiredTags() const
{
    return ActivationRequiredTags;
}

void URTSGameplayAbility::GetOrderTagRequirements(FRTSOrderTagRequirements& OutTagRequirements) const
{
    OutTagRequirements.SourceRequiredTags.AppendTags(SourceRequiredTags);
    OutTagRequirements.SourceBlockedTags.AppendTags(SourceBlockedTags);
    OutTagRequirements.TargetRequiredTags.AppendTags(TargetRequiredTags);
    OutTagRequirements.TargetBlockedTags.AppendTags(TargetBlockedTags);
}

void URTSGameplayAbility::GetSourceTagRequirements(FGameplayTagContainer& OutRequiredTags,
                                                   FGameplayTagContainer& OutBlockedTags) const
{
    OutRequiredTags.AppendTags(SourceRequiredTags);
    OutBlockedTags.AppendTags(SourceBlockedTags);
}

void URTSGameplayAbility::GetTargetTagRequirements(FGameplayTagContainer& OutRequiredTags,
                                                   FGameplayTagContainer& OutBlockedTags) const
{
    OutRequiredTags.AppendTags(TargetRequiredTags);
    OutBlockedTags.AppendTags(TargetBlockedTags);
}

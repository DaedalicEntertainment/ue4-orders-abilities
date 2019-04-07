#include "Orders/RTSUseAbilityOrder.h"

#include "OrdersAbilities.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility.h"
#include "GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"

#include "AbilitySystem/RTSAbilitySystemComponent.h"
#include "AbilitySystem/RTSAbilitySystemHelper.h"
#include "AbilitySystem/RTSGameplayAbility.h"
#include "AbilitySystem/RTSGlobalTags.h"
#include "Orders/RTSOrderErrorTags.h"
#include "Orders/RTSOrderProcessPolicy.h"
#include "Orders/RTSOrderTargetData.h"


URTSUseAbilityOrder::URTSUseAbilityOrder()
{
}

bool URTSUseAbilityOrder::CanObeyOrder(const AActor* OrderedActor, int32 Index,
                                       FRTSOrderErrorTags* OutErrorTags /*= nullptr*/) const
{
    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    const UGameplayAbility* Ability = GetAbility(AbilitySystem, Index);

    if (Ability != nullptr)
    {
        const TArray<FGameplayAbilitySpec>& AbilitySpecs = AbilitySystem->GetActivatableAbilities();
        for (const FGameplayAbilitySpec& Spec : AbilitySpecs)
        {
            if (Spec.Ability == Ability)
            {
                // Check if ability has been learned yet.
                if (Spec.Level <= 0)
                {
                    return false;
                }

                FGameplayTagContainer FailureTags;

                // Don't pass any source and target tags to can activate ability. These tags has already been checked in
                // 'URTSOrderHelper'. Only the activation required and activation blocked tags are checked here.
                if (!Ability->CanActivateAbility(Spec.Handle, AbilitySystem->AbilityActorInfo.Get(), nullptr, nullptr,
                                                 &FailureTags))
                {
                    if (OutErrorTags != nullptr)
                    {
                        OutErrorTags->ErrorTags = FailureTags;
                    }

                    return false;
                }

                // Not the nicest place to check this but it avoids adding this tag to every ability.
                if (AbilitySystem->HasMatchingGameplayTag(URTSGlobalTags::Status_Changing_Constructing()))
                {
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}

ERTSTargetType URTSUseAbilityOrder::GetTargetType(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return ERTSTargetType::NONE;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetTargetType();
    }

    return ERTSTargetType::NONE;
}

void URTSUseAbilityOrder::IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
                                     FRTSOrderCallback Callback, const FVector& HomeLocation) const
{
    if (OrderedActor == nullptr)
    {
        UE_LOG(LogRTS, Error, TEXT("Ordered actor is invalid."));
        Callback.Broadcast(ERTSOrderResult::FAILED);
        return;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    UGameplayAbility* Ability = GetAbility(AbilitySystem, Index);

    if (GetOrderProcessPolicy(OrderedActor, Index) == ERTSOrderProcessPolicy::INSTANT)
    {
        FGameplayEventData EventData;
        URTSAbilitySystemHelper::CreateGameplayEventData(OrderedActor, TargetData, Ability->GetClass(), EventData);

        int32 TriggeredAbilities = URTSAbilitySystemHelper::SendGameplayEvent(OrderedActor, EventData);
        if (TriggeredAbilities > 0)
        {
            Callback.Broadcast(ERTSOrderResult::SUCCEEDED);
        }

        else
        {
            Callback.Broadcast(ERTSOrderResult::FAILED);
        }
    }

    else
    {
        Super::IssueOrder(OrderedActor, TargetData, Index, Callback, HomeLocation);
    }
}

UTexture2D* URTSUseAbilityOrder::GetNormalIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

UTexture2D* URTSUseAbilityOrder::GetHoveredIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

UTexture2D* URTSUseAbilityOrder::GetPressedIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

UTexture2D* URTSUseAbilityOrder::GetDisabledIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

FText URTSUseAbilityOrder::GetName(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return FText::FromString(TEXT("URTSUseAbilityOrder::GetName: Error: Parameter 'OrderedActor' was 'nullptr'."));
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return FText::FromString(TEXT("URTSUseAbilityOrder::GetName: Error: Parameter 'Index' was invalid."));
    }

    return Ability->GetName();
}

FText URTSUseAbilityOrder::GetDescription(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return FText::FromString(
            TEXT("URTSUseAbilityOrder::GetDescription: Error: Parameter 'OrderedActor' was 'nullptr'."));
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return FText::FromString(TEXT("URTSUseAbilityOrder::GetName: Error: Parameter 'Index' was invalid."));
    }

    return Ability->GetDescription(OrderedActor);
}

FRTSOrderPreviewData URTSUseAbilityOrder::GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return FRTSOrderPreviewData();
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return FRTSOrderPreviewData();
    }

    return Ability->GetAbilityPreviewData();
}

ERTSOrderProcessPolicy URTSUseAbilityOrder::GetOrderProcessPolicy(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return Super::GetOrderProcessPolicy(OrderedActor, Index);
    }

    URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return Super::GetOrderProcessPolicy(OrderedActor, Index);
    }

    ERTSAbilityProcessPolicy AbilityProcessPolicy = Ability->GetAbilityProcessPolicy();

    // If this ability does not have a target type or location and is instant we can execute it here directly without
    // altering the AI behavior.
    if ((Ability->GetTargetType() == ERTSTargetType::NONE || Ability->GetTargetType() == ERTSTargetType::PASSIVE) &&
        AbilityProcessPolicy == ERTSAbilityProcessPolicy::INSTANT)
    {
        return ERTSOrderProcessPolicy::INSTANT;
    }

    else if (AbilityProcessPolicy == ERTSAbilityProcessPolicy::CAN_NOT_BE_CANCELED)
    {
        return ERTSOrderProcessPolicy::CAN_NOT_BE_CANCELED;
    }

    else if (AbilityProcessPolicy == ERTSAbilityProcessPolicy::CAN_BE_CANCELED)
    {
        return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
    }

    else if (AbilityProcessPolicy == ERTSAbilityProcessPolicy::CAN_BE_CANCELED_WHEN_NO_GAMEPLAY_TASK_IS_RUNNING)
    {
        if (Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
        {
            // It should not be possible to run ability tasks using a non instanced ability.
            return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
        }

        FGameplayAbilitySpec AbilitySpec;

        const TArray<FGameplayAbilitySpec>& AbilitySpecs = AbilitySystem->GetActivatableAbilities();
        for (const FGameplayAbilitySpec& Spec : AbilitySpecs)
        {
            if (Spec.Ability == Ability)
            {
                AbilitySpec = Spec;
                break;
            }
        }

        if (AbilitySpec.Ability == nullptr)
        {
            return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
        }

        // Iterate through every active instance to check if any of them has an active ability task.
        TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
        for (UGameplayAbility* AbilityInstance : AbilityInstances)
        {
            URTSGameplayAbility* RTSAbilityInstance = Cast<URTSGameplayAbility>(AbilityInstance);

            if (AbilityInstance != nullptr && RTSAbilityInstance->AreAbilityTasksActive())
            {
                return ERTSOrderProcessPolicy::CAN_NOT_BE_CANCELED;
            }
        }

        return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
    }

    return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
}

ERTSOrderGroupExecutionType URTSUseAbilityOrder::GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return ERTSOrderGroupExecutionType::ALL;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetGroupExecutionType();
    }

    return ERTSOrderGroupExecutionType::ALL;
}

bool URTSUseAbilityOrder::IsHumanPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->IsHumanPlayerAutoAbility();
    }

    return false;
}

bool URTSUseAbilityOrder::GetHumanPlayerAutoOrderInitialState(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetHumanPlayerAutoAutoAbilityInitialState();
    }

    return false;
}

bool URTSUseAbilityOrder::IsAIPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->IsAIPlayerAutoAbility();
    }

    return false;
}

bool URTSUseAbilityOrder::GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index,
                                                       float& OutAcquisitionRadius) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetAcquisitionRadiusOverride(OutAcquisitionRadius);
    }

    return false;
}

float URTSUseAbilityOrder::GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return Super::GetTargetScore(OrderedActor, TargetData, Index);
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability == nullptr || !Ability->IsTargetScoreOverriden())
    {
        Super::GetTargetScore(OrderedActor, TargetData, Index);
    }

    float TargetScore;
    Ability->GetTargetScore(OrderedActor, TargetData, Index, TargetScore);
    return TargetScore;
}

void URTSUseAbilityOrder::GetTagRequirements(const AActor* OrderedActor, int32 Index,
                                             FRTSOrderTagRequirements& OutTagRequirements) const
{
    if (OrderedActor == nullptr)
    {
        return;
    }

    URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetOrderTagRequirements(OutTagRequirements);
    }
}

float URTSUseAbilityOrder::GetRequiredRange(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return 0.0f;
    }

    URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    UGameplayAbility* Ability = GetAbility(AbilitySystem, Index);

    if (Ability != nullptr)
    {
        return AbilitySystem->GetAbilityRange(Ability->GetClass());
    }

    return 0.0f;
}

UGameplayAbility* URTSUseAbilityOrder::GetAbility(const URTSAbilitySystemComponent* AbilitySystem, int32 Index) const
{
    const TArray<TSubclassOf<UGameplayAbility>>& Abilities = AbilitySystem->GetInitialAndUnlockableAbilities();

    if (Abilities.IsValidIndex(Index) && Abilities[Index] != nullptr)
    {
        return Abilities[Index]->GetDefaultObject<UGameplayAbility>();
    }

    return nullptr;
}

UTexture2D* URTSUseAbilityOrder::GetIcon(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return nullptr;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return nullptr;
    }

    return Ability->GetIcon();
}

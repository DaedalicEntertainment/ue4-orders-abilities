#include "Orders/RTSOrderHelper.h"

#include "OrdersAbilities.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Templates/Tuple.h"

#include "AbilitySystem/RTSAbilitySystemHelper.h"
#include "AbilitySystem/RTSGlobalTags.h"
#include "Orders/RTSAutoOrderComponent.h"
#include "Orders/RTSOrderComponent.h"
#include "Orders/RTSOrderTargetData.h"
#include "Orders/RTSOrderWithBehavior.h"


bool URTSOrderHelper::CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index)
{
    return CanObeyOrder(OrderType, OrderedActor, Index, nullptr);
}

bool URTSOrderHelper::K2_CanObeyOrderWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                   int32 Index, FRTSOrderErrorTags& OutErrorTags)
{
    // Clear the tag container. If this does not happen you may get old values when using this function from blueprint.
    OutErrorTags.MissingTags.Reset();
    OutErrorTags.BlockingTags.Reset();
    OutErrorTags.ErrorTags.Reset();

    return CanObeyOrder(OrderType, OrderedActor, Index, &OutErrorTags);
}

bool URTSOrderHelper::CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                                   FRTSOrderErrorTags* OutErrorTags)
{

    if (OrderType == nullptr || !IsValid(OrderedActor))
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();
    const UAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<UAbilitySystemComponent>();
    if (AbilitySystem != nullptr)
    {
        FRTSOrderTagRequirements TagRequirements;
        Order->GetTagRequirements(OrderedActor, Index, TagRequirements);

        FGameplayTagContainer OrderedActorTags;
        AbilitySystem->GetOwnedGameplayTags(OrderedActorTags);

        if (OutErrorTags != nullptr)
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(
                    OrderedActorTags, TagRequirements.SourceRequiredTags, TagRequirements.SourceBlockedTags,
                    OutErrorTags->MissingTags, OutErrorTags->BlockingTags))
            {
                return false;
            }
        }
        else
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(
                    OrderedActorTags, TagRequirements.SourceRequiredTags, TagRequirements.SourceBlockedTags))
            {
                return false;
            }
        }
    }

    return Order->CanObeyOrder(OrderedActor, Index, OutErrorTags);
}

bool URTSOrderHelper::IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                    const FRTSOrderTargetData& TargetData, int32 Index)
{
    return IsValidTarget(OrderType, OrderedActor, TargetData, Index, nullptr);
}

bool URTSOrderHelper::K2_IsValidTargetWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                    const FRTSOrderTargetData& TargetData, int32 Index,
                                                    FRTSOrderErrorTags& OutErrorTags)
{
    // Clear the tag container. If this does not happen you may get old values when using this function from blueprint.
    OutErrorTags.MissingTags.Reset();
    OutErrorTags.BlockingTags.Reset();
    OutErrorTags.ErrorTags.Reset();

    return IsValidTarget(OrderType, OrderedActor, TargetData, Index, &OutErrorTags);
}

bool URTSOrderHelper::IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                    const FRTSOrderTargetData& TargetData, int32 Index,
                                    FRTSOrderErrorTags* OutErrorTags)
{
    if (OrderType == nullptr)
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();

    ERTSTargetType TargetType = Order->GetTargetType(OrderedActor, Index);
    if (TargetType == ERTSTargetType::ACTOR)
    {
        if (!IsValid(TargetData.Actor))
        {
            if (OutErrorTags != nullptr)
            {
                OutErrorTags->ErrorTags.AddTag(URTSGlobalTags::AbilityActivationFailure_NoTarget());
            }
            return false;
        }

        FRTSOrderTagRequirements TagRequirements;
        Order->GetTagRequirements(OrderedActor, Index, TagRequirements);

        if (OutErrorTags != nullptr)
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(
                    TargetData.TargetTags, TagRequirements.TargetRequiredTags, TagRequirements.TargetBlockedTags,
                    OutErrorTags->MissingTags, OutErrorTags->BlockingTags))
            {
                return false;
            }
        }

        else
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(
                    TargetData.TargetTags, TagRequirements.TargetRequiredTags, TagRequirements.TargetBlockedTags))
            {
                return false;
            }
        }
    }

    return Order->IsValidTarget(OrderedActor, TargetData, Index, OutErrorTags);
}

void URTSOrderHelper::CreateIndividualTargetLocations(TSoftClassPtr<URTSOrder> OrderType,
                                                      const TArray<AActor*>& OrderedActors,
                                                      const FRTSOrderTargetData& TargetData,
                                                      TArray<FVector2D>& OutTargetLocations)
{
    if (OrderType == nullptr)
    {
        return;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    OrderType->GetDefaultObject<URTSOrder>()->CreateIndividualTargetLocations(OrderedActors, TargetData,
                                                                              OutTargetLocations);

    if (OrderedActors.Num() != OutTargetLocations.Num())
    {
        UE_LOG(LogRTS, Error,
               TEXT("The implementation of 'CreateIndividualTargetLocations' of class '%s' does return an amount of "
                    "locations that is different to the amount of actors."),
               *OrderType->GetName());
    }
}

void URTSOrderHelper::IssueOrder(AActor* OrderedActor, const FRTSOrderData& Order)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::IssueOrder: The specified actor is invalid."));
        return;
    }

    URTSOrderComponent* OrderComponent = OrderedActor->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::IssueOrder: The specified actor has no order component."));
        return;
    }

    OrderComponent->IssueOrder(Order);
}

void URTSOrderHelper::ClearOrderQueue(AActor* OrderedActor)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::EnqueueOrder: The specified actor is invalid."));
        return;
    }

    URTSOrderComponent* OrderComponent = OrderedActor->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::EnqueueOrder: The specified actor has no order component."));
        return;
    }

    OrderComponent->ClearOrderQueue();
}

void URTSOrderHelper::EnqueueOrder(AActor* OrderedActor, const FRTSOrderData& Order)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::EnqueueOrder: The specified actor is invalid."));
        return;
    }

    URTSOrderComponent* OrderComponent = OrderedActor->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::EnqueueOrder: The specified actor has no order component."));
        return;
    }

    OrderComponent->EnqueueOrder(Order);
}

void URTSOrderHelper::InsertOrderAfterCurrentOrder(AActor* OrderedActor, const FRTSOrderData& Order)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::InsertOrderAfterCurrentOrder: The specified actor is invalid."));
        return;
    }

    URTSOrderComponent* OrderComponent = OrderedActor->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        UE_LOG(LogRTS, Error,
               TEXT("URTSOrderHelper::InsertOrderAfterCurrentOrder: The specified actor has no order component."));
        return;
    }

    OrderComponent->InsertOrderAfterCurrentOrder(Order);
}

ERTSTargetType URTSOrderHelper::GetTargetType(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                              int32 Index)
{
    if (OrderType == nullptr)
    {
        return ERTSTargetType::NONE;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetTargetType(OrderedActor, Index);
}

bool URTSOrderHelper::IsCreatingIndividualTargetLocations(TSoftClassPtr<URTSOrder> OrderType,
                                                          const AActor* OrderedActor, int32 Index)
{
    if (OrderType == nullptr)
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->IsCreatingIndividualTargetLocations(OrderedActor, Index);
}

UBehaviorTree* URTSOrderHelper::GetBehaviorTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrderWithBehavior>()->GetBehaviorTree();
}

bool URTSOrderHelper::ShouldRestartBehaviourTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType)
{
    if (OrderType == nullptr)
    {
        return true;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrderWithBehavior>()->ShouldRestartBehaviourTree();
}

FRTSOrderTargetData URTSOrderHelper::CreateOrderTargetData(const AActor* OrderedActor, AActor* TargetActor,
                                                           const FVector2D& TargetLocation)
{
    FRTSOrderTargetData TargetData;
    TargetData.Actor = TargetActor;
    TargetData.Location = TargetLocation;

    if (TargetActor == nullptr)
    {
        return TargetData;
    }

    FGameplayTagContainer SourceTags;
    FGameplayTagContainer TargetTags;
    URTSAbilitySystemHelper::GetSourceAndTargetTags(OrderedActor, TargetActor, SourceTags, TargetTags);

    TargetData.TargetTags = TargetTags;
    return TargetData;
}

UTexture2D* URTSOrderHelper::GetNormalIcon(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor /*= nullptr*/,
                                           int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetNormalIcon(OrderedActor, Index);
}

UTexture2D* URTSOrderHelper::GetHoveredIcon(TSoftClassPtr<URTSOrder> OrderType,
                                            const AActor* OrderedActor /*= nullptr*/, int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetHoveredIcon(OrderedActor, Index);
}

UTexture2D* URTSOrderHelper::GetPressedIcon(TSoftClassPtr<URTSOrder> OrderType,
                                            const AActor* OrderedActor /*= nullptr*/, int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetPressedIcon(OrderedActor, Index);
}

UTexture2D* URTSOrderHelper::GetDisabledIcon(TSoftClassPtr<URTSOrder> OrderType,
                                             const AActor* OrderedActor /*= nullptr*/, int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetDisabledIcon(OrderedActor, Index);
}

FText URTSOrderHelper::GetName(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor /*= nullptr*/,
                               int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return FText::FromString(TEXT("URTSOrderHelper::GetName: Error: Parameter 'OrderType' was 'nullptr'."));
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetName(OrderedActor, Index);
}

FText URTSOrderHelper::GetDescription(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor /*= nullptr*/,
                                      int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return FText::FromString(TEXT("URTSOrderHelper::GetDescription: Error: Parameter 'OrderType' was 'nullptr'."));
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetDescription(OrderedActor, Index);
}

int32 URTSOrderHelper::GetOrderButtonIndex(TSoftClassPtr<URTSOrder> OrderType)
{
    if (OrderType == nullptr)
    {
        return -1;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetOrderButtonIndex();
}

bool URTSOrderHelper::HasFixedOrderButtonIndex(TSoftClassPtr<URTSOrder> OrderType)
{
    if (OrderType == nullptr)
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->HasFixedOrderButtonIndex();
}

FRTSOrderPreviewData URTSOrderHelper::GetOrderPreviewData(TSoftClassPtr<URTSOrder> OrderType,
                                                          const AActor* OrderedActor /*= nullptr*/,
                                                          int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return FRTSOrderPreviewData();
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetOrderPreviewData(OrderedActor, Index);
}

void URTSOrderHelper::GetOrderTagRequirements(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                              int32 Index, FRTSOrderTagRequirements& OutTagRequirements)
{
    if (OrderType == nullptr)
    {
        UE_LOG(LogRTS, Error,
               TEXT("URTSOrderHelper::GetOrderTagRequirements: Error: Parameter 'OrderType' was 'nullptr'."));
        return;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetTagRequirements(OrderedActor, Index, OutTagRequirements);
}

void URTSOrderHelper::GetOrderSuccessTagRequirements(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                     int32 Index, FRTSOrderTagRequirements& OutTagRequirements)
{
    if (OrderType == nullptr)
    {
        UE_LOG(LogRTS, Error,
               TEXT("URTSOrderHelper::GetOrderTagRequirements: Error: Parameter 'OrderType' was 'nullptr'."));
        return;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetSuccessTagRequirements(OrderedActor, Index, OutTagRequirements);
}

float URTSOrderHelper::GetOrderRequiredRange(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                             int32 Index)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::GetOrderRequiredRange: The specified actor is invalid."));
        return 0.0f;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetRequiredRange(OrderedActor, Index);
}

bool URTSOrderHelper::GetAcquisitionRadiusOverride(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                   int32 Index, float& OutAcquisitionRadius)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error,
               TEXT("URTSOrderHelper::GetOrderAcquisitionRadiusOverride: The specified actor is invalid."));
        return 0.0f;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetAcquisitionRadiusOverride(OrderedActor, Index,
                                                                                  OutAcquisitionRadius);
}

ERTSOrderProcessPolicy URTSOrderHelper::GetOrderProcessPolicy(TSoftClassPtr<URTSOrder> OrderType,
                                                              const AActor* OrderedActor, int32 Index /*= -1*/)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::IsInstantOrder: The specified actor is invalid."));
        return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
    }

    if (OrderType == nullptr)
    {
        return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetOrderProcessPolicy(OrderedActor, Index);
}

TSoftClassPtr<URTSOrder> URTSOrderHelper::GetFallbackOrder(TSoftClassPtr<URTSOrder> OrderType)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetFallbackOrder();
}

float URTSOrderHelper::GetOrderTargetScore(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                           const FRTSOrderTargetData& TargetData, int32 Index)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::GetOrderTargetScore: The specified actor is invalid."));
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetTargetScore(OrderedActor, TargetData, Index);
}

bool URTSOrderHelper::IsHumanPlayerAutoOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                             int32 Index /*= -1*/)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::IsHumanPlayerAutoOrder: The specified actor is invalid."));
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->IsHumanPlayerAutoOrder(OrderedActor, Index);
}

bool URTSOrderHelper::IsAIPlayerAutoOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                          int32 Index /*= -1*/)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::IsAIPlayerAutoOrder: The specified actor is invalid."));
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->IsAIPlayerAutoOrder(OrderedActor, Index);
}

bool URTSOrderHelper::GetHumanPlayerAutoOrderInitialState(TSoftClassPtr<URTSOrder> OrderType,
                                                          const AActor* OrderedActor, int32 Index /*= -1*/)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error,
               TEXT("URTSOrderHelper::GetHumanPlayerAutoOrderInitialState: The specified actor is invalid."));
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetHumanPlayerAutoOrderInitialState(OrderedActor, Index);
}

void URTSOrderHelper::SetHumanPlayerAutoOrderState(const AActor* OrderedActor, const FRTSOrderTypeWithIndex& Order,
                                                   bool bEnable)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogRTS, Error, TEXT("URTSOrderHelper::SetHumanPlayerAutoOrderState: The specified actor is invalid."));
        return;
    }

    URTSAutoOrderComponent* AutoOrderComponent = OrderedActor->FindComponentByClass<URTSAutoOrderComponent>();

    if (!IsValid(AutoOrderComponent))
    {
        UE_LOG(
            LogRTS, Error,
            TEXT("URTSOrderHelper::SetHumanPlayerAutoOrderState: The specified actor has no RTSAutoOrderComponent."));
        return;
    }

    AutoOrderComponent->SetHumanPlayerAutoOrderState(Order, bEnable);
}

bool URTSOrderHelper::AreAutoOrdersAllowedDuringOrder(TSoftClassPtr<URTSOrder> OrderType)
{
    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->AreAutoOrdersAllowedDuringOrder();
}

bool URTSOrderHelper::CanOrderBeConsideredAsSucceeded(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                      const FRTSOrderTargetData& TargetData, int32 Index /*= -1*/)
{
    if (OrderType == nullptr || !IsValid(OrderedActor))
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();
    const UAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<UAbilitySystemComponent>();

    check(AbilitySystem != nullptr);

    FRTSOrderTagRequirements TagRequirements;
    Order->GetSuccessTagRequirements(OrderedActor, Index, TagRequirements);

    FGameplayTagContainer OrderedActorTags;
    AbilitySystem->GetOwnedGameplayTags(OrderedActorTags);

    if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(OrderedActorTags, TagRequirements.SourceRequiredTags,
                                                             TagRequirements.SourceBlockedTags))
    {
        return false;
    }

    if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(TargetData.TargetTags, TagRequirements.TargetRequiredTags,
                                                             TagRequirements.TargetBlockedTags))
    {
        return false;
    }

    return true;
}

ERTSOrderGroupExecutionType URTSOrderHelper::GetOrderGroupExecutionType(TSoftClassPtr<URTSOrder> OrderType,
                                                                        const AActor* OrderedActor,
                                                                        int32 Index /*= -1*/)
{
    if (OrderType == nullptr)
    {
        return ERTSOrderGroupExecutionType::ALL;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetGroupExecutionType(OrderedActor, Index);
}

FRTSOrderErrorTags URTSOrderHelper::CheckOrder(AActor* OrderedActor, const FRTSOrderData& Order)
{
    // TODO: Detailed check with callback with parameters that describe why the order could not be issued.

    FRTSOrderErrorTags OrderErrorTags;

    if (!Order.OrderType.IsValid())
    {
        Order.OrderType.LoadSynchronous();
    }

    TSubclassOf<URTSOrder> OrderType = Order.OrderType.Get();
    if (OrderType == nullptr)
    {
        UE_LOG(LogRTS, Error,
               TEXT("URTSOrderComponent::CheckOrder: The specified order for the actor '%s' is invalid."),
               *OrderedActor->GetName());
        return OrderErrorTags;
    }

    if (!URTSOrderHelper::CanObeyOrder(OrderType.Get(), OrderedActor, Order.Index, &OrderErrorTags))
    {
        return OrderErrorTags;
    }

    FRTSOrderTargetData TargetData = URTSOrderHelper::CreateOrderTargetData(OrderedActor, Order.Target, Order.Location);
    if (!URTSOrderHelper::IsValidTarget(OrderType.Get(), OrderedActor, TargetData, Order.Index, &OrderErrorTags))
    {
        return OrderErrorTags;
    }

    return OrderErrorTags;
}

bool URTSOrderHelper::IsEnemyInAcquisitionRadius(const AActor* OrderedActor, float AcquisitionRadius)
{
    if (!IsValid(OrderedActor))
    {
        return false;
    }

    TArray<AActor*> ActorsInRange;
    FindActors(OrderedActor->GetWorld(), AcquisitionRadius, OrderedActor->GetActorLocation(), ActorsInRange);

    for (AActor* Actor : ActorsInRange)
    {
        if (!IsValid(Actor))
        {
            continue;
        }

        // Check the target tags.
        if (URTSAbilitySystemHelper::GetRelationshipTags(OrderedActor, Actor)
                .HasTag(URTSGlobalTags::Relationship_Hostile()))
        {
            return true;
        }
    }

    return false;
}

AActor* URTSOrderHelper::FindTargetForOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                                            float AcquisitionRadius, float& OutScore)
{
    if (!IsValid(OrderedActor))
    {
        return nullptr;
    }

    return FindTargetForOrderInChaseDistance(OrderType, OrderedActor, Index, AcquisitionRadius, AcquisitionRadius,
                                             OrderedActor->GetActorLocation(), OutScore);
}

AActor* URTSOrderHelper::FindTargetForOrderInChaseDistance(TSoftClassPtr<URTSOrder> OrderType,
                                                           const AActor* OrderedActor, int32 Index,
                                                           float AcquisitionRadius, float ChaseDistance,
                                                           const FVector& OrderedActorHomeLocation, float& OutScore)
{
    if (!IsValid(OrderedActor))
    {
        return nullptr;
    }

    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();

    // Only target types with a real target location are relevant.
    ERTSTargetType TargetType = Order->GetTargetType(OrderedActor, Index);
    if (TargetType == ERTSTargetType::NONE || TargetType == ERTSTargetType::PASSIVE)
    {
        return nullptr;
    }

    // Find all targets in acquisition radius.
    TArray<AActor*> ActorsInRange;
    FindActorsInChaseDistance(OrderedActor->GetWorld(), AcquisitionRadius, ChaseDistance,
                              OrderedActor->GetActorLocation(), OrderedActorHomeLocation, ActorsInRange);

    if (ActorsInRange.Num() == 0)
    {
        return nullptr;
    }

    return FindBestScoredTargetForOrder(OrderType, OrderedActor, ActorsInRange, Index, OutScore);
}

void URTSOrderHelper::FindActors(UObject* WorldContextObject, float AcquisitionRadius,
                                 const FVector& OrderedActorLocation, TArray<AActor*>& OutActorsInRange)
{
    FindActorsInChaseDistance(WorldContextObject, AcquisitionRadius, AcquisitionRadius, OrderedActorLocation,
                              OrderedActorLocation, OutActorsInRange);
}

void URTSOrderHelper::FindActorsInChaseDistance(UObject* WorldContextObject, float AcquisitionRadius,
                                                float ChaseDistance, const FVector& OrderedActorLocation,
                                                const FVector& OrderedActorHomeLocation,
                                                TArray<AActor*>& OutActorsInRange)
{
    TArray<AActor*> ActorsInRange;
    TArray<AActor*> ActorsToIgnore;

    // NOTE(np): In A Year Of Rain, we're storing detection channels for units in the game instance.
    // Find all targets in acquisition radius.
    /*URTSGameInstance* GameInstance = URTSUtilities::GetRTSGameInstance(WorldContextObject);
    UKismetSystemLibrary::CapsuleOverlapActors(WorldContextObject, OrderedActorLocation, AcquisitionRadius, 10000.0f,
                                               GameInstance->GetPawnDetectionChannels(), APawn::StaticClass(),
                                               ActorsToIgnore, ActorsInRange);*/

    if (ActorsInRange.Num() == 0)
    {
        return;
    }

    // Filter the array for valid targets.
    for (AActor* Actor : ActorsInRange)
    {
        if (!IsValid(Actor))
        {
            continue;
        }

        FVector ActorLocation = Actor->GetActorLocation();

        // If the target is too far away from our home location it is invalid.
        float DistanceToHomeLocation = FVector::Dist2D(ActorLocation, OrderedActorHomeLocation);
        if (DistanceToHomeLocation > ChaseDistance)
        {
            continue;
        }

        OutActorsInRange.Add(Actor);
    }
}

AActor* URTSOrderHelper::FindBestScoredTargetForOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                      const TArray<AActor*> Targets, int32 Index, float& OutScore)
{
    if (!IsValid(OrderedActor))
    {
        return nullptr;
    }

    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();

    // Filter the array for valid targets.
    FRTSOrderTagRequirements TagRequirements;
    Order->GetTagRequirements(OrderedActor, Index, TagRequirements);
    TArray<TTuple<AActor*, float>> ActorsWithScore;
    for (AActor* Actor : Targets)
    {
        if (!IsValid(Actor))
        {
            continue;
        }

        // Check the target tags.
        FGameplayTagContainer TargetTags;
        URTSAbilitySystemHelper::GetTags(Actor, TargetTags);
        TargetTags.AppendTags(URTSAbilitySystemHelper::GetRelationshipTags(OrderedActor, Actor));
        if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(TargetTags, TagRequirements.TargetRequiredTags,
                                                                 TagRequirements.TargetBlockedTags))
        {
            continue;
        }

        // Apply the order specific valid target check.
        FRTSOrderTargetData OrderTargetData(Actor, FVector2D(Actor->GetActorLocation()), TargetTags);
        if (!Order->IsValidTarget(OrderedActor, OrderTargetData, Index))
        {
            continue;
        }

        // This actor is valid. Store it and its score in the array.
        ActorsWithScore.Emplace(Actor, Order->GetTargetScore(OrderedActor, OrderTargetData, Index));
    }

    // Find the best best target out of all potential targets using the score.
    TTuple<AActor*, float> HighestScoredActor;
    for (TTuple<AActor*, float> ActorWithScore : ActorsWithScore)
    {
        if (HighestScoredActor.Get<1>() < ActorWithScore.Get<1>())
        {
            HighestScoredActor = ActorWithScore;
        }
    }

    OutScore = HighestScoredActor.Get<1>();
    return HighestScoredActor.Get<0>();
}

AActor* URTSOrderHelper::FindMostSuitableActorToObeyTheOrder(TSoftClassPtr<URTSOrder> OrderType,
                                                             const TArray<AActor*> OrderedActors,
                                                             const FRTSOrderTargetData TargetData, int32 OrderIndex)
{
    TArray<TTuple<AActor*, float>> ActorsWithScore;
    for (AActor* Actor : OrderedActors)
    {
        if (URTSOrderHelper::CanObeyOrder(OrderType, Actor, OrderIndex))
        {

            // This actor is valid. Store it and its score in the array.
            ActorsWithScore.Emplace(Actor, GetOrderTargetScore(OrderType, Actor, TargetData, OrderIndex));
        }
    }

    if (ActorsWithScore.Num() == 0)
    {
        return nullptr;
    }

    // Find the best best actor out of all potential selected actors using the score.
    TTuple<AActor*, float> HighestScoredActor = ActorsWithScore[0];

    for (TTuple<AActor*, float> ActorWithScore : ActorsWithScore)
    {
        if (HighestScoredActor.Get<1>() < ActorWithScore.Get<1>())
        {
            HighestScoredActor = ActorWithScore;
        }
    }

    return HighestScoredActor.Get<0>();
}

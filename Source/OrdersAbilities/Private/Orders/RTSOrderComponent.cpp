#include "Orders/RTSOrderComponent.h"

#include "OrdersAbilities.h"

#include "UnrealNetwork.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

#include "AbilitySystem/RTSAbilitySystemHelper.h"
#include "AbilitySystem/RTSGlobalTags.h"
#include "Orders/RTSCharacterAIController.h"
#include "Orders/RTSOrder.h"
#include "Orders/RTSOrderErrorTags.h"
#include "Orders/RTSOrderHelper.h"
#include "Orders/RTSStopOrder.h"


URTSOrderComponent::URTSOrderComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsReplicated(true);

    LastOrderHomeLocation = FVector::ZeroVector;
    bIsHomeLocationSet = false;
}

void URTSOrderComponent::BeginPlay()
{
    Super::BeginPlay();

    APawn* Pawn = Cast<APawn>(GetOwner());
    if (Pawn == nullptr)
    {
        return;
    }

    // NOTE(np): In A Year Of Rain, we're showing order previews for selected units.
    //// Register for selection events.
    //SelectableComponent = Pawn->FindComponentByClass<URTSSelectableComponent>();

    //if (SelectableComponent != nullptr)
    //{
    //    SelectableComponent->OnSelected.AddDynamic(this, &URTSOrderComponent::OnSelected);
    //    SelectableComponent->OnDeselected.AddDynamic(this, &URTSOrderComponent::OnDeselected);
    //}

    // Reset current order.
    CurrentOrder = FRTSOrderData();

    // Try to set the stop order if possible.
    ARTSCharacterAIController* Controller = Cast<ARTSCharacterAIController>(Pawn->GetController());
    if (Controller == nullptr)
    {
        return;
    }

    StopOrder = Controller->GetStopOrder();

    CurrentOrder = StopOrder;
    IssueOrder(StopOrder);
}

void URTSOrderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URTSOrderComponent, CurrentOrder);
    DOREPLIFETIME(URTSOrderComponent, OrderQueue);
}

void URTSOrderComponent::SetCurrentOrder(FRTSOrderData NewOrder)
{
    LastOrder = CurrentOrder;

    CurrentOrder = NewOrder;

    NotifyOnOrderChanged(CurrentOrder);
}

void URTSOrderComponent::NotifyOnOrderChanged(const FRTSOrderData& NewOrder)
{
    UpdateOrderPreviews();

    OnOrderChanged.Broadcast(NewOrder);
}

void URTSOrderComponent::ReceivedCurrentOrder()
{
    NotifyOnOrderChanged(CurrentOrder);
}

void URTSOrderComponent::ReceivedOrderQueue()
{
    UpdateOrderPreviews();
}

void URTSOrderComponent::IssueOrder(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();

    // It is impossible for clients to issue orders. Clients need to issue orders using their player controller.
    if (!Owner->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("The order %s was send from a client. It is impossible for clients to issue orders. Clients need "
                    "to issue orders using their player controller. "),
               *Order.ToString());
        return;
    }

    // Clear the order cue when another order is issued.
    OrderQueue.Empty();
    OnOrderQueueCleared.Broadcast();

    // Do nothing if we are obeying exact the same order already (and I mean exact: Not only the same order type)
    if (CurrentOrder == Order)
    {
        return;
    }

    if (!Order.OrderType.IsValid())
    {
        Order.OrderType.LoadSynchronous();
    }

    bIsHomeLocationSet = false;

    // Abort current order.
    if (CurrentOrder.OrderType != StopOrder &&
        URTSOrderHelper::GetOrderProcessPolicy(Order.OrderType, Owner, Order.Index) != ERTSOrderProcessPolicy::INSTANT)
    {
        switch (URTSOrderHelper::GetOrderProcessPolicy(CurrentOrder.OrderType, Owner, CurrentOrder.Index))
        {
            case ERTSOrderProcessPolicy::CAN_BE_CANCELED:
                OrderCanceled();

                if (CheckOrder(Order))
                {
                    ObeyOrder(Order);
                }
                else
                {
                    ObeyStopOrder();
                }
                break;
            case ERTSOrderProcessPolicy::CAN_NOT_BE_CANCELED:
                // We cannot cancel our current order so we need to queue it up as next.
                OrderQueue.Add(Order);
                break;
            case ERTSOrderProcessPolicy::INSTANT:
                // This should not be possible. Instant orders should not be set as current orders in the first place.
                check(0);
                break;
            default:
                check(0);
                break;
        }
    }

    else
    {
        if (CheckOrder(Order))
        {
            ObeyOrder(Order);
        }
        else
        {
            ObeyStopOrder();
        }
    }
}

void URTSOrderComponent::ClearOrderQueue()
{
    AActor* Owner = GetOwner();

    // It is impossible for clients to issue orders. Clients need to issue orders using their player controller.
    if (!Owner->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("It is impossible for clients to issue orders. Clients need "
                    "to issue orders using their player controller. "));
        return;
    }

    // Clear the order cue when another order is issued.
    OrderQueue.Empty();
    OnOrderQueueCleared.Broadcast();
}

void URTSOrderComponent::EnqueueOrder(const FRTSOrderData& Order)
{
    // It is impossible for clients to issue orders. Clients need to issue orders using their player controller.
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("The order %s was send from a client. It is impossible for clients to issue orders. Clients need "
                    "to issue orders using their player controller. "),
               *Order.ToString());
        return;
    }

    if (!CheckOrder(Order))
    {
        return;
    }

    if (OrderQueue.Num() == 0 && CurrentOrder.OrderType == StopOrder)
    {
        ObeyOrder(Order);
    }

    else
    {
        // Do nothing if we are obeying exact the same order already (and I mean exact: Not only the same order type)
        if (OrderQueue.Num() > 0 && OrderQueue.Last() == Order)
        {
            return;
        }

        OrderQueue.Add(Order);
        OnOrderEnqueued.Broadcast(Order);

        UpdateOrderPreviews();
    }
}

void URTSOrderComponent::InsertOrderAfterCurrentOrder(const FRTSOrderData& Order)
{
    // It is impossible for clients to issue orders. Clients need to issue orders using their player controller.
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("The order %s was send from a client. It is impossible for clients to issue orders. Clients need "
                    "to issue orders using their player controller. "),
               *Order.ToString());
        return;
    }

    if (!CheckOrder(Order))
    {
        return;
    }

    if (OrderQueue.Num() == 0 && CurrentOrder.OrderType == StopOrder)
    {
        ObeyOrder(Order);
    }

    bIsHomeLocationSet = false;

    OrderQueue.Insert(Order, 0);
}

void URTSOrderComponent::InsertOrderBeforeCurrentOrder(const FRTSOrderData& Order)
{
    // It is impossible for clients to issue orders. Clients need to issue orders using their player controller.
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("The order %s was send from a client. It is impossible for clients to issue orders. Clients need "
                    "to issue orders using their player controller. "),
               *Order.ToString());
        return;
    }

    if (!CheckOrder(Order))
    {
        return;
    }

    // Queue the current order.
    OrderQueue.Insert(CurrentOrder, 0);

    // Save home location of the current order.
    ARTSCharacterAIController* Controller = Cast<ARTSCharacterAIController>(Cast<APawn>(GetOwner())->GetController());
    if (Controller != nullptr)
    {
        LastOrderHomeLocation = Controller->GetHomeLocation();
    }

    // Directly obey the passed order.
    ObeyOrder(Order);

    // Set to true after obey order to not use the last order home location for the current order.
    bIsHomeLocationSet = true;
}

TSoftClassPtr<URTSOrder> URTSOrderComponent::GetCurrentOrderType() const
{
    return CurrentOrder.OrderType;
}

bool URTSOrderComponent::IsIdle() const
{
    return GetCurrentOrderType() == StopOrder;
}

FRTSOrderData URTSOrderComponent::GetCurrentOrderData() const
{
    return CurrentOrder;
}

TArray<FRTSOrderData> URTSOrderComponent::GetCurrentOrderDataQueue() const
{
    return OrderQueue;
}

AActor* URTSOrderComponent::GetCurrentOrderTargetActor() const
{
    return CurrentOrder.Target;
}

FVector2D URTSOrderComponent::GetCurrentOrderTargetLocation() const
{
    return CurrentOrder.Location;
}

int32 URTSOrderComponent::GetCurrentOrderTargetIndex() const
{
    return CurrentOrder.Index;
}

void URTSOrderComponent::ObeyOrder(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();
    FRTSOrderTargetData TargetData = URTSOrderHelper::CreateOrderTargetData(Owner, Order.Target, Order.Location);

    // Find the correct home location value for this order.
    FVector HomeLocation;
    if (bIsHomeLocationSet)
    {
        HomeLocation = LastOrderHomeLocation;
        bIsHomeLocationSet = false;
    }

    else
    {
        HomeLocation = Owner->GetActorLocation();
    }

    switch (URTSOrderHelper::GetOrderProcessPolicy(Order.OrderType, Owner, Order.Index))
    {
        case ERTSOrderProcessPolicy::INSTANT:
        {
            // An instant order is issued directly without changing the current order (ProductionOrder,
            // SetRallyPointOrder,
            // Some Abilities etc.).
            // Note: It is currently not possible to queue instant order because of the missing callback.
            // Maybe 'ObeyOrder' needs a return value that describes if the order is in progress or finished.
            Order.OrderType->GetDefaultObject<URTSOrder>()->IssueOrder(Owner, TargetData, Order.Index,
                                                                       FRTSOrderCallback(), HomeLocation);
        }
        break;
        case ERTSOrderProcessPolicy::CAN_BE_CANCELED:
        case ERTSOrderProcessPolicy::CAN_NOT_BE_CANCELED:
        {
            if (CurrentOrder.OrderType != StopOrder)

            {
                UnregisterTagListeners(CurrentOrder);
            }

            SetCurrentOrder(Order);

            FRTSOrderCallback Callback;
            Callback.AddDynamic(this, &URTSOrderComponent::OnOrderEndedCallback);

            if (!Order.OrderType.IsValid())
            {
                Order.OrderType.LoadSynchronous();
            }

            if (Order.OrderType != StopOrder)
            {
                RegisterTagListeners(Order);
            }

            Order.OrderType->GetDefaultObject<URTSOrder>()->IssueOrder(Owner, TargetData, Order.Index, Callback,
                                                                       HomeLocation);
        }
        break;
        default:
            check(0);
            break;
    }
}

bool URTSOrderComponent::CheckOrder(const FRTSOrderData& Order) const
{
    FRTSOrderErrorTags OrderErrorTags;

    AActor* OrderedActor = GetOwner();
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
        return false;
    }

    if (!URTSOrderHelper::CanObeyOrder(OrderType.Get(), OrderedActor, Order.Index, &OrderErrorTags))
    {
        LogOrderErrorMessage(
            FString::Printf(TEXT("URTSOrderComponent::CheckOrder: The actor '%s' cannot obey the order '%s'."),
                            *OrderedActor->GetName(), *OrderType->GetName()),
            OrderErrorTags);
        return false;
    }

    FRTSOrderTargetData TargetData = URTSOrderHelper::CreateOrderTargetData(OrderedActor, Order.Target, Order.Location);
    if (!URTSOrderHelper::IsValidTarget(OrderType.Get(), OrderedActor, TargetData, Order.Index, &OrderErrorTags))
    {
        LogOrderErrorMessage(
            FString::Printf(
                TEXT("URTSOrderComponent::CheckOrder: The actor '%s' was issued to obey the order '%s', but the "
                     "target data is invalid: %s"),
                *OrderedActor->GetName(), *OrderType->GetName(), *TargetData.ToString()),
            OrderErrorTags);
        return false;
    }

    return true;
}

void URTSOrderComponent::LogOrderErrorMessage(const FString& Message, const FRTSOrderErrorTags& OrderErrorTags) const
{
    // TODO: Better formatting?
    FString FinalMessage = Message + TEXT(": ") + OrderErrorTags.ToString();
    UE_LOG(LogRTS, Warning, TEXT("%s"), *FinalMessage);
}

void URTSOrderComponent::OnOrderEndedCallback(ERTSOrderResult OrderResult)
{
    OrderEnded(OrderResult);
}

void URTSOrderComponent::OrderEnded(ERTSOrderResult OrderResult)
{
    AActor* Owner = GetOwner();

    switch (OrderResult)
    {
        case ERTSOrderResult::FAILED:
            if (StopOrder != nullptr)
            {
                // OrderCanceled will be raised in IssueOrder.
                IssueOrder(FRTSOrderData(StopOrder));
            }
            return;
        case ERTSOrderResult::CANCELED:
            if (!URTSOrderHelper::CanOrderBeConsideredAsSucceeded(
                    CurrentOrder.OrderType, Owner,
                    URTSOrderHelper::CreateOrderTargetData(Owner, CurrentOrder.Target, CurrentOrder.Location),
                    CurrentOrder.Index))
            {
                // OrderCanceled will be raised in IssueOrder.
                IssueOrder(FRTSOrderData(StopOrder));
                break;
            }
            // Fall through if succeeded
        case ERTSOrderResult::SUCCEEDED:
            if (OrderQueue.IsValidIndex(0))
            {
                const FRTSOrderData NewOrder = OrderQueue[0];

                if (CheckOrder(NewOrder))
                {
                    OrderQueue.RemoveAt(0);
                    ObeyOrder(NewOrder);
                    return;
                }

                OrderQueue.Empty();
                ObeyStopOrder();
            }

            else if (StopOrder != nullptr)
            {
                ObeyStopOrder();
            }

            return;
    }
}

void URTSOrderComponent::OrderCanceled()
{
    AActor* Owner = GetOwner();
    FRTSOrderTargetData TargetData =
        URTSOrderHelper::CreateOrderTargetData(Owner, CurrentOrder.Target, CurrentOrder.Location);

    UClass* OrderType = nullptr;

    if (!CurrentOrder.OrderType.IsValid())
    {
        OrderType = CurrentOrder.OrderType.LoadSynchronous();
    }
    else
    {
        OrderType = CurrentOrder.OrderType.Get();
    }

    if (OrderType != nullptr)
    {
        OrderType->GetDefaultObject<URTSOrder>()->OrderCanceled(Owner, TargetData, CurrentOrder.Index);
    }
}

void URTSOrderComponent::RegisterTagListeners(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(Order.OrderType, Owner, Order.Index, TagRequirements);

    UAbilitySystemComponent* OwnerAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

    // Owner tags
    //

    if (OwnerAbilitySystem != nullptr)
    {
        FGameplayTagContainer OwnerTags;
        for (FGameplayTag Tag : TagRequirements.SourceRequiredTags)
        {
            // Don't register a delegate for permanent status tags.
            if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
            {
                OwnerTags.AddTagFast(Tag);
            }
        }

        for (FGameplayTag Tag : TagRequirements.SourceBlockedTags)
        {
            // Don't register a delegate for permanent status tags.
            if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
            {
                OwnerTags.AddTag(Tag);
            }
        }

        // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
        if (TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
        {
            OwnerTags.AddTag(URTSGlobalTags::Status_Changing_Detector());
        }

        // Register a callback for each of the tags to check if it was added to or removed.
        for (FGameplayTag Tag : OwnerTags)
        {
            FOnGameplayEffectTagCountChanged& Delegate =
                OwnerAbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);

            FDelegateHandle DelegateHandle = Delegate.AddUObject(this, &URTSOrderComponent::OnOwnerTagsChanged);
            RegisteredOwnerTagEventHandles.Add(Tag, DelegateHandle);
        }
    }

    // Target tags
    //

    ERTSTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType, Owner, Order.Index);
    if (TargetType == ERTSTargetType::ACTOR)
    {
        UAbilitySystemComponent* TargetAbilitySystem =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Order.Target);

        if (TargetAbilitySystem != nullptr)
        {
            FGameplayTagContainer TargetTags;
            for (FGameplayTag Tag : TagRequirements.TargetRequiredTags)
            {
                // Don't register a delegate for permanent status tags.
                if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
                {
                    TargetTags.AddTagFast(Tag);
                }
            }

            for (FGameplayTag Tag : TagRequirements.TargetBlockedTags)
            {
                // Don't register a delegate for permanent status tags.
                if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
                {
                    TargetTags.AddTag(Tag);
                }
            }

            // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
            if (TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
            {
                TargetTags.AddTag(URTSGlobalTags::Status_Changing_Stealthed());
            }

            // Register a callback for each of the tags to check if it was added to or removed.
            for (FGameplayTag Tag : TargetTags)
            {
                FOnGameplayEffectTagCountChanged& Delegate =
                    TargetAbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);

                FDelegateHandle DelegateHandle = Delegate.AddUObject(this, &URTSOrderComponent::OnTargetTagsChanged);
                RegisteredTargetTagEventHandles.Add(Tag, DelegateHandle);
            }
        }
    }
}

void URTSOrderComponent::UnregisterTagListeners(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(Order.OrderType, Owner, Order.Index, TagRequirements);

    UAbilitySystemComponent* OwnerAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

    // Owner tags
    //

    if (OwnerAbilitySystem != nullptr)
    {
        for (TPair<FGameplayTag, FDelegateHandle> Pair : RegisteredOwnerTagEventHandles)
        {
            FOnGameplayEffectTagCountChanged& Delegate =
                OwnerAbilitySystem->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved);

            Delegate.Remove(Pair.Value);
        }

        RegisteredOwnerTagEventHandles.Empty();
    }

    // Target tags
    //

    ERTSTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType, Owner, Order.Index);
    if (TargetType == ERTSTargetType::ACTOR)
    {
        UAbilitySystemComponent* TargetAbilitySystem =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Order.Target);

        if (TargetAbilitySystem != nullptr)
        {
            for (TPair<FGameplayTag, FDelegateHandle> Pair : RegisteredTargetTagEventHandles)
            {
                FOnGameplayEffectTagCountChanged& Delegate =
                    TargetAbilitySystem->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved);

                Delegate.Remove(Pair.Value);
            }

            RegisteredTargetTagEventHandles.Empty();
        }
    }
}

void URTSOrderComponent::OnTargetTagsChanged(const FGameplayTag Tag, int32 NewCount)
{
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(CurrentOrder.OrderType, GetOwner(), CurrentOrder.Index, TagRequirements);

    if ((NewCount && TagRequirements.TargetBlockedTags.HasTag(Tag)) ||
        (!NewCount && TagRequirements.TargetRequiredTags.HasTag(Tag)))
    {
        OrderEnded(ERTSOrderResult::CANCELED);
    }

    // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
    else if (NewCount && Tag == URTSGlobalTags::Status_Changing_Stealthed() &&
             TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
    {
        if (!URTSAbilitySystemHelper::IsVisibleForActor(GetOwner(), CurrentOrder.Target))
        {
            OrderEnded(ERTSOrderResult::CANCELED);
        }
    }
}

void URTSOrderComponent::OnOwnerTagsChanged(const FGameplayTag Tag, int32 NewCount)
{
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(CurrentOrder.OrderType, GetOwner(), CurrentOrder.Index, TagRequirements);

    if ((NewCount && TagRequirements.SourceBlockedTags.HasTag(Tag)) ||
        !NewCount && TagRequirements.SourceRequiredTags.HasTag(Tag))
    {
        OrderEnded(ERTSOrderResult::CANCELED);
    }

    // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
    else if (!NewCount && Tag == URTSGlobalTags::Status_Changing_Detector() &&
             TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
    {
        if (!URTSAbilitySystemHelper::IsVisibleForActor(GetOwner(), CurrentOrder.Target))
        {
            OrderEnded(ERTSOrderResult::CANCELED);
        }
    }
}

void URTSOrderComponent::ObeyStopOrder()
{
    if (StopOrder == nullptr)
    {
        UE_LOG(LogRTS, Error, TEXT("%s has no stop order set."), *GetOwner()->GetName());
        return;
    }

    if (!StopOrder.IsValid())
    {
        StopOrder.LoadSynchronous();
    }

    ObeyOrder(FRTSOrderData(StopOrder));
}

AActor* URTSOrderComponent::CreateOrderPreviewActor(const FRTSOrderData& Order)
{
    if (OrderPreviewActorClass == nullptr)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Instigator = Cast<APawn>(GetOwner());
    SpawnInfo.ObjectFlags |= RF_Transient;

    FVector Location;
    AActor* SpawnedActor = nullptr;

    // Spawn default order preview.
    ERTSTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType);
    switch (TargetType)
    {
        case ERTSTargetType::NONE:
            break;
        case ERTSTargetType::ACTOR:
            if (Order.Target != nullptr)
            {
                Location = Order.Target->GetActorLocation();
                SpawnedActor =
                    GetWorld()->SpawnActor<AActor>(OrderPreviewActorClass, Location, FRotator::ZeroRotator, SpawnInfo);

                SpawnedActor->AttachToActor(Order.Target, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                OrderPreviewActors.Add(SpawnedActor);
            }
            break;
        case ERTSTargetType::LOCATION:
        case ERTSTargetType::DIRECTION:
            // NOTE(np): In A Year Of Rain, we're using a raycast to translate between 3D and 2D space.
            //Location = URTSUtilities::GetGroundLocation2D(this, Order.Location);
            Location = FVector(Order.Location.X, Order.Location.Y, 0.0f);
            SpawnedActor =
                GetWorld()->SpawnActor<AActor>(OrderPreviewActorClass, Location, FRotator::ZeroRotator, SpawnInfo);
            OrderPreviewActors.Add(SpawnedActor);
            break;
        case ERTSTargetType::PASSIVE:
            break;
        default:
            break;
    }

    return SpawnedActor;
}

void URTSOrderComponent::OnSelected()
{
    UpdateOrderPreviews();
}

void URTSOrderComponent::OnDeselected()
{
    // Destroy previews except for construction previews.
    for (int32 Index = OrderPreviewActors.Num() - 1; Index > 0; --Index)
    {
        if (OrderPreviewActors.IsValidIndex(Index) && OrderQueue.IsValidIndex(Index - 1) &&
            OrderQueue[Index - 1].OrderType != BeginConstructionOrder)
        {
            OrderPreviewActors[Index]->Destroy();
            OrderPreviewActors.RemoveAt(Index);
        }
    }

    if (OrderPreviewActors.IsValidIndex(0) && CurrentOrder.OrderType != BeginConstructionOrder)
    {
        OrderPreviewActors[0]->Destroy();
        OrderPreviewActors.RemoveAt(0);
    }
}

void URTSOrderComponent::UpdateOrderPreviews()
{
    // Clear old previews.
    for (AActor* OrderPreviewActor : OrderPreviewActors)
    {
        if (IsValid(OrderPreviewActor))
        {
            OrderPreviewActor->Destroy();
        }
    }

    OrderPreviewActors.Empty();

    // Don't spawn order previews for actors of others.
    AActor* Owner = GetOwner();

    if (!IsValid(Owner))
    {
        return;
    }

    // NOTE(np): In A Year Of Rain, we're only showing order previews for friendly players.
    /*URTSOwnerComponent* OwnerComponent = Owner->FindComponentByClass<URTSOwnerComponent>();

    if (!IsValid(OwnerComponent))
    {
        return;
    }

    if (!OwnerComponent->IsSameTeamAsController(UGameplayStatics::GetPlayerController(this, 0)))
    {
        return;
    }

    // Spawn new previews.
    bool bSelected = SelectableComponent != nullptr && SelectableComponent->IsSelected();

    if ((bSelected && OrderQueue.Num() > 0) || CurrentOrder.OrderType == BeginConstructionOrder)
    {
        CreateOrderPreviewActor(CurrentOrder);
    }

    for (const FRTSOrderData& OrderData : OrderQueue)
    {
        if (bSelected || OrderData.OrderType == BeginConstructionOrder)
        {
            CreateOrderPreviewActor(OrderData);
        }
    }
    */
}

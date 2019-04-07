#include "Orders/RTSAutoOrderComponent.h"

#include "OrdersAbilities.h"

#include "UnrealNetwork.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "Orders/RTSAutoOrderProvider.h"
#include "Orders/RTSOrderHelper.h"
#include "Orders/RTSOrderComponent.h"

DECLARE_CYCLE_STAT(TEXT("RTS - Auto Order Target Acquisition"), STAT_RTSAutoOrderTargetAcquisition, STATGROUP_RTS);


URTSAutoOrderComponent::URTSAutoOrderComponent()
{
    SetIsReplicated(true);

    bCheckAutoOrders = false;
}

void URTSAutoOrderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URTSAutoOrderComponent, HumanPlayerAutoOrderStates);
}

bool URTSAutoOrderComponent::IsHumanPlayerAutoOrder(const FRTSOrderTypeWithIndex& Order)
{
    for (int32 i = 0; i < Orders.Num(); ++i)
    {
        if (Order == Orders[i])
        {
            return HumanPlayerAutoOrders[i];
        }
    }

    return false;
}

void URTSAutoOrderComponent::SetHumanPlayerAutoOrderState(const FRTSOrderTypeWithIndex& Order, bool bEnable)
{
    for (int32 i = 0; i < Orders.Num(); ++i)
    {
        if (Order == Orders[i])
        {
            HumanPlayerAutoOrderStates[i] = bEnable;
            if (bEnable)
            {
                bCheckAutoOrders = true;
            }

            break;
        }
    }
}

bool URTSAutoOrderComponent::GetHumanPlayerAutoOrderState(const FRTSOrderTypeWithIndex& Order)
{
    for (int32 i = 0; i < Orders.Num(); ++i)
    {
        if (Order == Orders[i])
        {
            return HumanPlayerAutoOrderStates[i];
        }
    }

    return false;
}

void URTSAutoOrderComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();

    if (!IsValid(Owner))
    {
        return;
    }

    URTSOrderComponent* OrderComponent = Owner->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        return;
    }

    // Find all auto orders.
    UClass* ProviderInterfaceClass = URTSAutoOrderProvider::StaticClass();
    if (Owner->GetClass()->ImplementsInterface(ProviderInterfaceClass))
    {
        IRTSAutoOrderProvider::Execute_GetAutoOrders(Owner, Orders);
    }

    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->GetClass()->ImplementsInterface(ProviderInterfaceClass))
        {
            IRTSAutoOrderProvider::Execute_GetAutoOrders(Component, Orders);
        }
    }

    if (Orders.Num() == 0)
    {
        return;
    }

    HumanPlayerAutoOrders.AddDefaulted(Orders.Num());

    // Only certain abilities are auto cast abilities for human players.
    for (int32 i = 0; i < Orders.Num(); ++i)
    {
        HumanPlayerAutoOrders[i] = URTSOrderHelper::IsHumanPlayerAutoOrder(Orders[i].OrderType, Owner, Orders[i].Index);
    }

    // Calculating states and AI data is done on server-side, only.
    if (!Owner->HasAuthority())
    {
        return;
    }

    HumanPlayerAutoOrderStates.AddDefaulted(Orders.Num());
    AIPlayerAutoOrders.AddDefaulted(Orders.Num());

    bool bHasAutoCastOrders = false;

    for (int32 i = 0; i < Orders.Num(); ++i)
    {
        if (HumanPlayerAutoOrders[i])
        {
            bool bInitialEnabled =
                URTSOrderHelper::GetHumanPlayerAutoOrderInitialState(Orders[i].OrderType, Owner, Orders[i].Index);
            bHasAutoCastOrders |= bInitialEnabled;
            HumanPlayerAutoOrderStates[i] = bInitialEnabled;
        }

        AIPlayerAutoOrders[i] = URTSOrderHelper::IsAIPlayerAutoOrder(Orders[i].OrderType, Owner, Orders[i].Index);
        bHasAutoCastOrders |= AIPlayerAutoOrders[i];
    }

    // NOTE(np): In A Year Of Rain, AI units have auto cast enabled by default.
    //if (OwnerComponent->GetPlayerOwner() == nullptr || URTSUtilities::IsAIUnit(Owner))
    //{
    //    bHasAutoCastOrders = true;
    //}

    // Listen for the appropriate order to enable auto orders.
    OrderComponent->OnOrderChanged.AddDynamic(this, &URTSAutoOrderComponent::OnOrderChanged);

    // NOTE(np): In A Year Of Rain, units can change their owner at runtime (e.g. rescued units in story campaign).
    //URTSOwnerComponent* OwnerComponent = Owner->FindComponentByClass<URTSOwnerComponent>();
    //if (OwnerComponent != nullptr)
    //{
    //    // Listen for owner changes.
    //    OwnerComponent->OnOwnerChanged.AddDynamic(this, &URTSAutoOrderComponent::OnOwnerChanged);
    //}

    if (bHasAutoCastOrders && URTSOrderHelper::AreAutoOrdersAllowedDuringOrder(OrderComponent->GetCurrentOrderType()))
    {
        bCheckAutoOrders = true;
    }

    // NOTE(np): A Year Of Rain registers the actor with a manager for bucketed updates here, to improve performance.
    //// Register actor with game.
    //ARTSGameMode* GameMode = Cast<ARTSGameMode>(UGameplayStatics::GetGameMode(this));

    //if (IsValid(GameMode))
    //{
    //    GameMode->AddAutoOrderActor(Owner);
    //}
}

void URTSAutoOrderComponent::CheckAutoOrders()
{
    if (!bCheckAutoOrders)
    {
        return;
    }

    AActor* Owner = GetOwner();

    // NOTE(np): A Year Of Rain distingushes between auto orders for human and AI.
    //bool bIsAIUnit = URTSUtilities::IsAIUnit(Owner);
    bool bIsAIUnit = false;

    {
        SCOPE_CYCLE_COUNTER(STAT_RTSAutoOrderTargetAcquisition);

        for (int32 i = 0; i < Orders.Num(); ++i)
        {
            if ((bIsAIUnit && AIPlayerAutoOrders[i]) ||
                (!bIsAIUnit && HumanPlayerAutoOrders[i] && HumanPlayerAutoOrderStates[i]))
            {
                FRTSOrderTypeWithIndex Order = Orders[i];
                if (IssueAutoOrder(Order))
                {
                    break;
                }
            }
        }
    }
}

void URTSAutoOrderComponent::OnOrderChanged(const FRTSOrderData& NewOrder)
{
    bCheckAutoOrders = URTSOrderHelper::AreAutoOrdersAllowedDuringOrder(NewOrder.OrderType);
}

void URTSAutoOrderComponent::OnOwnerChanged(APlayerState* PreviousOwner, APlayerState* NewOwner)
{
    if (!IsValid(NewOwner))
    {
        return;
    }

    URTSOrderComponent* OrderComponent = GetOwner()->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        return;
    }

    bool bHasAutoCastOrders = false;

    // For AI players
    if (NewOwner->bIsABot)
    {
        // Every ability is an auto cast ability for AI players.
        bHasAutoCastOrders = Orders.Num() > 0;
    }

    // For human players
    else
    {
        // Only certain abilities are auto cast abilities for human players.
        for (int32 i = 0; i < Orders.Num(); ++i)
        {
            if (HumanPlayerAutoOrders[i] && HumanPlayerAutoOrderStates[i])
            {
                bHasAutoCastOrders = true;
                break;
            }
        }
    }

    if (bHasAutoCastOrders)
    {
        bCheckAutoOrders = URTSOrderHelper::AreAutoOrdersAllowedDuringOrder(OrderComponent->GetCurrentOrderType());
    }
}

bool URTSAutoOrderComponent::IssueAutoOrder(const FRTSOrderTypeWithIndex& Order)
{
    float AcquisitionRadius = GetAcquisitionRadius(Order);

    AActor* Owner = GetOwner();
    if (!URTSOrderHelper::CanObeyOrder(Order.OrderType, GetOwner(), Order.Index))
    {
        return false;
    }

    URTSOrderComponent* OrderComponent = Owner->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        return false;
    }

    ERTSTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType, Owner, Order.Index);
    switch (TargetType)
    {
        case ERTSTargetType::NONE:
        {
            // TODO: Orders with no target type (this basically resolves to 'self') are only issued when their is an
            // enemy nearby. This might not always be the case (e.g self only heal).
            if (URTSOrderHelper::IsEnemyInAcquisitionRadius(Owner, AcquisitionRadius))
            {
                OrderComponent->InsertOrderBeforeCurrentOrder(FRTSOrderData(Order.OrderType, Order.Index));
                return true;
            }
        }
        break;
        case ERTSTargetType::ACTOR:
        case ERTSTargetType::LOCATION:
        case ERTSTargetType::DIRECTION:
        {
            float Score;
            AActor* Target =
                URTSOrderHelper::FindTargetForOrder(Order.OrderType, Owner, Order.Index, AcquisitionRadius, Score);
            if (Target != nullptr)
            {
                OrderComponent->InsertOrderBeforeCurrentOrder(
                    FRTSOrderData(Order.OrderType, Order.Index, Target, FVector2D(Target->GetActorLocation())));
                return true;
            }
        }
        break;
        case ERTSTargetType::PASSIVE:
            break;
        default:
            break;
    }

    return false;
}

float URTSAutoOrderComponent::GetAcquisitionRadius(const FRTSOrderTypeWithIndex& Order)
{
    AActor* Owner = GetOwner();

    float AcquisitionRadius = 0.0f;
    if (URTSOrderHelper::GetAcquisitionRadiusOverride(Order.OrderType, Owner, Order.Index, AcquisitionRadius))
    {
        return AcquisitionRadius;
    }

    // NOTE(np): A Year Of Rain returns the default acquisition radius of the units here.

    /*URTSAttackComponent* AttackComponent = Owner->FindComponentByClass<URTSAttackComponent>();
    if (AttackComponent == nullptr)
    {
        return 0.0f;
    }

    return AttackComponent->GetAcquisitionRadius();*/
    return 0.0f;
}

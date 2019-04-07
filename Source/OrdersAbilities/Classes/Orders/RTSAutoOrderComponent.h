#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Orders/RTSOrderTypeWithIndex.h"
#include "Orders/RTSOrderData.h"
#include "RTSAutoOrderComponent.generated.h"

class APlayerState;

/**
 * Manages the orders of a unit that should be issued automatically.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ORDERSABILITIES_API URTSAutoOrderComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URTSAutoOrderComponent();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Whether the specified order is a togglable auto order for the human player. */
    UFUNCTION(Category = RTS, BlueprintPure)
    bool IsHumanPlayerAutoOrder(const FRTSOrderTypeWithIndex& Order);

    /** Sets whether the specified auto order is enabled to be issued automatically. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void SetHumanPlayerAutoOrderState(const FRTSOrderTypeWithIndex& Order, bool bEnable);

    /** Gets whether the the specified auto order is enabled to be issued automatically. */
    UFUNCTION(Category = RTS, BlueprintPure)
    bool GetHumanPlayerAutoOrderState(const FRTSOrderTypeWithIndex& Order);

    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    //~ End UActorComponent Interface

    void CheckAutoOrders();

private:
    UFUNCTION()
    void OnOrderChanged(const FRTSOrderData& NewOrder);

    UFUNCTION()
    void OnOwnerChanged(APlayerState* PreviousOwner, APlayerState* NewOwner);

    bool IssueAutoOrder(const FRTSOrderTypeWithIndex& Order);
    float GetAcquisitionRadius(const FRTSOrderTypeWithIndex& Order);

    /** Contains all orders that may be issued automatically with their associated index. */
    TArray<FRTSOrderTypeWithIndex> Orders;

    /** Whether an auto order is a togglable auto order for the human player.  */
    // TODO: Are these values really needed. The 'Orders' array should only contain orders that can be issued
    // automatically by human players. It should only contain more orders for AI players.
    TArray<bool> HumanPlayerAutoOrders;

    /** Whether an order should automatically be activated by AI players.  */
    TArray<bool> AIPlayerAutoOrders;

    /** Whether an auto order is enabled to be issued automatically. */
    UPROPERTY(replicated)
    TArray<bool> HumanPlayerAutoOrderStates;

    bool bCheckAutoOrders;
};
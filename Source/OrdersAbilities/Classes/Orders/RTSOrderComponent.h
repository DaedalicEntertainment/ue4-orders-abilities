#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Orders/RTSOrderData.h"
#include "Orders/RTSOrderResult.h"
#include "RTSOrderComponent.generated.h"

class URTSSelectableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSOrderComponentOrderEnqueuedSignature, const FRTSOrderData&, Order);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRTSOrderComponentOrderQueueClearedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSOrderComponentOrderChangedSignature, const FRTSOrderData&, NewOrder);

/**
 * Manages the orders for a RTSChracter or RTSPawn
 */
UCLASS()
class ORDERSABILITIES_API URTSOrderComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URTSOrderComponent(const FObjectInitializer& ObjectInitializer);

    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    //~ Begin UActorComponent Interface

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Event when the actor has received a new order. */
    virtual void NotifyOnOrderChanged(const FRTSOrderData& NewOrder);

    /** Event when the actor has received a new order. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
    FRTSOrderComponentOrderChangedSignature OnOrderChanged;

    /** Issues this unit to obey the specified order. */
    void IssueOrder(const FRTSOrderData& Order);

    /** Enqueues an order that will be issued to the unit if all other orders has succeeded. */
    void EnqueueOrder(const FRTSOrderData& Order);

    /** Inserts an order that will be issued when this current order has succeeded. */
    void InsertOrderAfterCurrentOrder(const FRTSOrderData& Order);

    /** Clears the order queue. */
    void ClearOrderQueue();

    /**
     * Inserts an order that will be issued immediately. The current order will be ordered again when this order
     * finishes.
     */
    void InsertOrderBeforeCurrentOrder(const FRTSOrderData& Order);

    /** Gets the type of the current order of this pawn. */
    UFUNCTION(Category = RTS, BlueprintPure)
    TSoftClassPtr<URTSOrder> GetCurrentOrderType() const;

    /** Checks whether the pawn is idle, or has any orders. */
    UFUNCTION(Category = RTS, BlueprintPure)
    bool IsIdle() const;

    FRTSOrderData GetCurrentOrderData() const;

    TArray<FRTSOrderData> GetCurrentOrderDataQueue() const;

    /** Gets the target actor of the current order of this pawn. */
    UFUNCTION(Category = RTS, BlueprintPure)
    AActor* GetCurrentOrderTargetActor() const;

    /** Gets the target location of the current order of this pawn. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FVector2D GetCurrentOrderTargetLocation() const;

    /** Gets the target index of the current order of this pawn. */
    UFUNCTION(Category = RTS, BlueprintPure)
    int32 GetCurrentOrderTargetIndex() const;

    /** Event when a new order has been enqueued. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
    FRTSOrderComponentOrderEnqueuedSignature OnOrderEnqueued;

    /** Event when the order queue has been cleared. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
    FRTSOrderComponentOrderQueueClearedSignature OnOrderQueueCleared;

private:
    UPROPERTY(BlueprintReadOnly, Category = "RTS", ReplicatedUsing = ReceivedCurrentOrder,
              meta = (AllowPrivateAccess = true))
    FRTSOrderData CurrentOrder;

    UPROPERTY(BlueprintReadOnly, Category = "RTS", ReplicatedUsing = ReceivedCurrentOrder,
              meta = (AllowPrivateAccess = true))
    FRTSOrderData LastOrder;

    UPROPERTY(BlueprintReadOnly, Category = "RTS", ReplicatedUsing = ReceivedOrderQueue,
              meta = (AllowPrivateAccess = true))
    TArray<FRTSOrderData> OrderQueue;

    UPROPERTY()
    TSoftClassPtr<URTSOrder> StopOrder;

    UPROPERTY()
    TArray<AActor*> OrderPreviewActors;

    /** Class of the preview actor that is used to show the target location of an order. */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    TSubclassOf<AActor> OrderPreviewActorClass;

    /** Order type that is used to begin the construction of a building. */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    TSoftClassPtr<URTSOrder> BeginConstructionOrder;

    URTSSelectableComponent* SelectableComponent;

    /**
     * The handles of the delegates that are registered on the ability system of the actor owner to be able to abort
     * the order if the requirements are nor longer fulfilled.
     */
    TMap<FGameplayTag, FDelegateHandle> RegisteredOwnerTagEventHandles;

    /**
     * The handles of the delegates that are registered on the ability system of the actor target to be able to abort
     * the order if the requirements are nor longer fulfilled.
     */
    TMap<FGameplayTag, FDelegateHandle> RegisteredTargetTagEventHandles;

    /** Last order home location if set. */
    FVector LastOrderHomeLocation;

    /** Indicates whether 'LastOrderHomeLocation' has a valid value. */
    bool bIsHomeLocationSet;

    /** Store the current order of the actor. */
    void SetCurrentOrder(FRTSOrderData NewOrder);

    UFUNCTION()
    void ReceivedCurrentOrder();

    UFUNCTION()
    void ReceivedOrderQueue();

    void ObeyOrder(const FRTSOrderData& Order);
    bool CheckOrder(const FRTSOrderData& Order) const;
    void LogOrderErrorMessage(const FString& Message, const FRTSOrderErrorTags& OrderErrorTags) const;

    UFUNCTION()
    void OnOrderEndedCallback(ERTSOrderResult OrderResult);

    void OrderEnded(ERTSOrderResult OrderResult);
    void OrderCanceled();
    void RegisterTagListeners(const FRTSOrderData& Order);
    void UnregisterTagListeners(const FRTSOrderData& Order);

    UFUNCTION()
    void OnTargetTagsChanged(const FGameplayTag Tag, int32 NewCount);

    UFUNCTION()
    void OnOwnerTagsChanged(const FGameplayTag Tag, int32 NewCount);

    void ObeyStopOrder();

    AActor* CreateOrderPreviewActor(const FRTSOrderData& Order);

    UFUNCTION()
    void OnSelected();

    UFUNCTION()
    void OnDeselected();

    void UpdateOrderPreviews();
};

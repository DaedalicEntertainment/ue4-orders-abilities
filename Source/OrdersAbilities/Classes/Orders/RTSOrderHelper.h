#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Text.h"
#include "Vector2D.h"
#include "Orders/RTSOrder.h"
#include "Orders/RTSOrderData.h"
#include "Orders/RTSOrderErrorTags.h"
#include "Orders/RTSOrderPreviewData.h"
#include "Orders/RTSOrderProcessPolicy.h"
#include "Orders/RTSOrderTargetData.h"
#include "Orders/RTSOrderTypeWithIndex.h"
#include "RTSOrderHelper.generated.h"

class AActor;
class UTexture2D;
class URTSOrderWithBehavior;

/**
 * Helper functions for accessing the default objects of order classes.
 */
UCLASS()
class ORDERSABILITIES_API URTSOrderHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Whether the specified actor can obey this kind of order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index = -1);

    /** Whether the specified actor can obey this kind of order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure, DisplayName = "Can Obey Order (With Error Tags)")
    static bool K2_CanObeyOrderWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                             int32 Index, FRTSOrderErrorTags& OutErrorTags);

    /** Whether the specified actor can obey this kind of order. */
    static bool CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                             FRTSOrderErrorTags* OutErrorTags);

    /** Whether the specified actor and/or location is a valid target for this order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                              const FRTSOrderTargetData& TargetData, int32 Index = -1);

    /** Whether the specified actor and/or location is a valid target for this order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure, DisplayName = "Is Valid Target (With Error Tags)")
    static bool K2_IsValidTargetWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                              const FRTSOrderTargetData& TargetData, int32 Index,
                                              FRTSOrderErrorTags& OutErrorTags);

    /** Whether the specified actor and/or location is a valid target for this order. */
    static bool IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                              const FRTSOrderTargetData& TargetData, int32 Index, FRTSOrderErrorTags* OutErrorTags);

    /** Creates individual target locations for the group of actors. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static void CreateIndividualTargetLocations(TSoftClassPtr<URTSOrder> OrderType,
                                                const TArray<AActor*>& OrderedActors,
                                                const FRTSOrderTargetData& TargetData,
                                                TArray<FVector2D>& OutTargetLocations);

    /** Issues the specified actor to obey this order on the specified target. */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void IssueOrder(AActor* OrderedActor, const FRTSOrderData& Order);

    /** Clears the order of the specified actor. Should probably only be used if queuing orders immediately after. */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void ClearOrderQueue(AActor* OrderedActor);

    /** Enqueues an order that will be issued to the specified actor if all its other orders has succeeded. */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void EnqueueOrder(AActor* OrderedActor, const FRTSOrderData& Order);

    /** Inserts an order that will be issued when this current order of the specified actor has succeeded. */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void InsertOrderAfterCurrentOrder(AActor* OrderedActor, const FRTSOrderData& Order);

    /** Gets the target type the specified order is using. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static ERTSTargetType GetTargetType(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr,
                                        int32 Index = -1);

    /** Whether this order type creates individual target locations for a group of actors */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool IsCreatingIndividualTargetLocations(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                    int32 Index);

    /** Gets the behavior tree that should run in order to obey this order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static UBehaviorTree* GetBehaviorTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType);

    /** Whether to restart the behaviour tree whenever a new order of the specified type is issued. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool ShouldRestartBehaviourTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType);

    /** Creates order target data using the specified target actor. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static FRTSOrderTargetData CreateOrderTargetData(const AActor* OrderedActor, AActor* TargetActor,
                                                     const FVector2D& TargetLocation = FVector2D::ZeroVector);

    /** Gets the normal icon of the order. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static UTexture2D* GetNormalIcon(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr,
                                     int32 Index = -1);

    /** Gets the hovered icon of the order. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static UTexture2D* GetHoveredIcon(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr,
                                      int32 Index = -1);

    /** Gets the pressed icon of the order. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static UTexture2D* GetPressedIcon(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr,
                                      int32 Index = -1);

    /** Gets the disabled icon of the order. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static UTexture2D* GetDisabledIcon(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr,
                                       int32 Index = -1);

    /** Gets the name of the order. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static FText GetName(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr, int32 Index = -1);

    /** Gets the name of the order. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static FText GetDescription(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor = nullptr,
                                int32 Index = -1);

    /** Gets the index of the button when shown in the UI. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static int32 GetOrderButtonIndex(TSoftClassPtr<URTSOrder> OrderType);

    /** Checks whether to use a fixed index of the button when shown in the UI, instead of just lining it up among
     * others. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool HasFixedOrderButtonIndex(TSoftClassPtr<URTSOrder> OrderType);

    /** Gets details about the preview for the order while choosing a target. */
    UFUNCTION(Category = RTS, BlueprintPure)
    static FRTSOrderPreviewData GetOrderPreviewData(TSoftClassPtr<URTSOrder> OrderType,
                                                    const AActor* OrderedActor = nullptr, int32 Index = -1);

    /** Gets the tag requirements for this order that must be full filled to be issued. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static void GetOrderTagRequirements(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                                        FRTSOrderTagRequirements& OutTagRequirements);

    /** Gets the tag requirements for this order that must be full filled to be successful. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static void GetOrderSuccessTagRequirements(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                               int32 Index, FRTSOrderTagRequirements& OutTagRequirements);

    /**
     * Gets the required range between the ordered actor and the target that is needed to fulfill the order.
     * '0' value is returned if the order has no required range.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static float GetOrderRequiredRange(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                       int32 Index = -1);

    /**
     * Gets the specified order specific acquisition radius for this order and returns whether the specified order uses
     * a specific acquisition radius.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool GetAcquisitionRadiusOverride(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                             int32 Index, float& OutAcquisitionRadius);

    /**
     * Gets a value that describes how the specified order is executed. This might determine how the order is displayed
     * in the UI and it determines how the order is handled by the order system.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static ERTSOrderProcessPolicy GetOrderProcessPolicy(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                        int32 Index = -1);

    /** Gets the order to issue instead if the player specified an invalid target for the specified one. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static TSoftClassPtr<URTSOrder> GetFallbackOrder(TSoftClassPtr<URTSOrder> OrderType);

    /**
     * Evaluates the target and returns a score that can be used to compare the different targets. A higher score means
     * a better target for the order.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static float GetOrderTargetScore(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                     const FRTSOrderTargetData& TargetData, int32 Index = -1);

    /** Whether the specified order is an auto order for the human player. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool IsHumanPlayerAutoOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                       int32 Index = -1);

    /** Whether the specified order is an auto order for AI players. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool IsAIPlayerAutoOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index = -1);

    /** When the specified order is an auto order, this value indicates whether it is active by default. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool GetHumanPlayerAutoOrderInitialState(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                    int32 Index = -1);

    /** Sets whether the specified auto order is enabled to be issued automatically. */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void SetHumanPlayerAutoOrderState(const AActor* OrderedActor, const FRTSOrderTypeWithIndex& Order,
                                             bool bEnable);

    /** Whether the specified order order allows auto orders when it is active. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool AreAutoOrdersAllowedDuringOrder(TSoftClassPtr<URTSOrder> OrderType);

    /**
     * Whether the specified order can be considered as succeeded in regard to the specified ordered actor and order
     * target.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool CanOrderBeConsideredAsSucceeded(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                const FRTSOrderTargetData& TargetData, int32 Index = -1);

    /** Gets the group execution type of the specified order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static ERTSOrderGroupExecutionType GetOrderGroupExecutionType(TSoftClassPtr<URTSOrder> OrderType,
                                                                  const AActor* OrderedActor, int32 Index = -1);

    /** Checks whether the specified actor can apply the passed order to its target. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static FRTSOrderErrorTags CheckOrder(AActor* OrderedActor, const FRTSOrderData& Order);

    /**
     * Checks if their is an actor that has a hostile relationship with the ordered actor inside the specified
     * acquisition radius.
     */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static bool IsEnemyInAcquisitionRadius(const AActor* OrderedActor, float AcquisitionRadius);

    /**
     * Finds the best matching target for the specified order inside the specified acquisition radius.
     * @param OrderType                 The order type.
     * @param OrderedActor              The ordered actor
     * @param Index                     Order index. This is needed for certain orders to differentiate. Default '-1'.
     * @param AcquisitionRadius         Max distance from the ordered actor to a potential target.
     * @param OutScore                  Score of the returned target if a target was found.
     * @return                          The target or 'nullptr' if no target was found.
     */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static AActor* FindTargetForOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                                      float AcquisitionRadius, float& OutScore);

    /**
     * Finds the best matching target for the specified order inside the specified acquisition radius and inside the
     * specified chase distance.
     * @param OrderType                 The order type.
     * @param OrderedActor              The ordered actor
     * @param Index                     Order index. This is needed for certain orders to differentiate. Default '-1'.
     * @param AcquisitionRadius         Max distance from the ordered actor to a potential target.
     * @param ChaseDistance             Max distance from the 'OrderedActorHomeLocation' to a potential target.
     * @param OrderedActorHomeLocation  Home location of the 'OrderedActor'. Used in conjunction with 'ChaseDistance'
     * @param OutScore                  Score of the returned target if a target was found.
     * @return                          The target or 'nullptr' if no target was found.
     */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static AActor* FindTargetForOrderInChaseDistance(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                     int32 Index, float AcquisitionRadius, float ChaseDistance,
                                                     const FVector& OrderedActorHomeLocation, float& OutScore);

    /**
     * Finds the most suitable actor to obey the specified order.
     * @param OrderType                 The order type.
     * @param OrderedActor              The actors to check.
     * @param TargetData                The target for the order.
     * @param Index                     Order index. This is needed for certain orders to differentiate. Default '-1'.
     * @return                          The target or 'nullptr' if no target was found.
     */
    static AActor* FindMostSuitableActorToObeyTheOrder(TSoftClassPtr<URTSOrder> OrderType,
                                                       const TArray<AActor*> OrderedActors,
                                                       const FRTSOrderTargetData TargetData, int32 Index);

    /**
     * Finds all actors inside the specified acquisition radius.
     * @param WorldContextObject        World context.
     * @param AcquisitionRadius         Max distance from the ordered actor to a potential target.
     * @param OrderedActorLocation      Current location of the ordered actor.
     * @param OutActorsInRange          All found actors in range.
     */
    static void FindActors(UObject* WorldContextObject, float AcquisitionRadius, const FVector& OrderedActorLocation,
                           TArray<AActor*>& OutActorsInRange);

    /**
     * Finds all actors inside the specified acquisition radius and inside the specified chase distance.
     * @param WorldContextObject        World context.
     * @param AcquisitionRadius         Max distance from the ordered actor to a potential target.
     * @param ChaseDistance             Max distance from the 'OrderedActorHomeLocation' to a potential target.
     * @param OrderedActorLocation      Current location of the ordered actor.
     * @param OrderedActorHomeLocation  Home location of the ordered actor'. Used in conjunction with 'ChaseDistance'
     * @param OutActorsInRange          All found actors in range.
     */
    static void FindActorsInChaseDistance(UObject* WorldContextObject, float AcquisitionRadius, float ChaseDistance,
                                          const FVector& OrderedActorLocation, const FVector& OrderedActorHomeLocation,
                                          TArray<AActor*>& OutActorsInRange);

    /**
     * Finds the best matching target for the specified order.
     * @param OrderType                 The order type.
     * @param OrderedActor              The ordered actor
     * @param Index                     Order index. This is needed for certain orders to differentiate. Default '-1'.
     * @param Targets                   All potential target actors.
     * @param OutScore                  Score of the returned target if a target was found.
     * @return                          The target or 'nullptr' if no target was found.
     */
    static AActor* FindBestScoredTargetForOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                const TArray<AActor*> Targets, int32 Index, float& OutScore);
};

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Orders/RTSOrderData.h"
#include "RTSCharacterAIController.generated.h"

class URTSAttackComponent;
class URTSOrder;
class URTSStopOrder;
class URTSGatherOrder;
class URTSContinueConstructionOrder;

/**
 * AI controller that drives RTS unit movement and orders.
 */
UCLASS()
class ORDERSABILITIES_API ARTSCharacterAIController : public AAIController
{
    GENERATED_BODY()

public:
    /** Constructor that takes an ObjectInitializer. */
    ARTSCharacterAIController(const FObjectInitializer& ObjectInitializer);

    /** Gets the building class of the current order. */
    UFUNCTION(BlueprintPure)
    TSubclassOf<AActor> GetBuildingClass() const;

    /** Checks whether the pawn has an order of the specified type. */
    UFUNCTION(BlueprintPure)
    bool HasOrder(TSubclassOf<URTSOrder> OrderType) const;

    /** Issues this unit to obey the specified order. */
    void IssueOrder(const FRTSOrderData& Order, FRTSOrderCallback Callback, const FVector& HomeLocation);

    //~ Begin AActor Interface
    virtual void Tick(float DeltaTime) override;
    //~ End AActor Interface

    UFUNCTION(Category = RTS, BlueprintPure)
    TSoftClassPtr<URTSStopOrder> GetStopOrder() const;

    /** Called from the behavior tree to indicate that it has ended with the passed result. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void BehaviorTreeEnded(EBTNodeResult::Type Result);

    /** Gets the current units home location from the black board. */
    FVector GetHomeLocation();

protected:
    virtual void Possess(APawn* InPawn) override;

private:
    /** Collision object types that are used to detect attack targets. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RTS", meta = (AllowPrivateAccess = true))
    TArray<TEnumAsByte<EObjectTypeQuery>> AttackTargetDetectionChannels;

    /** Collision object types that are used to find free locations. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RTS", meta = (AllowPrivateAccess = true))
    TArray<TEnumAsByte<EObjectTypeQuery>> FindLocationDetectionChannels;

    /** The order to stop the character and put him in the idle state. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RTS", meta = (AllowPrivateAccess = true))
    TSoftClassPtr<URTSStopOrder> StopOrder;

    /** Blackboard to use for holding all data relevant to the character AI. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RTS", meta = (AllowPrivateAccess = true))
    UBlackboardData* CharacterBlackboardAsset;

    TArray<FRTSOrderData> OrderQueue;

    FRTSOrderCallback CurrentOrderResultCallback;

    /** Just used to cache the result of a behavior tree */
    EBTNodeResult::Type BehaviorTreeResult;

    void SetBlackboardValues(const FRTSOrderData& Order, const FVector& HomeLocation);
    void ApplyOrder(const FRTSOrderData& Order, UBehaviorTree* BehaviorTree);

    bool VerifyBlackboard() const;
};

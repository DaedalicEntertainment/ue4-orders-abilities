#pragma once

#include "CoreMinimal.h"
#include "Text.h"
#include "UObject/NoExportTypes.h"
#include "Orders/RTSOrderGroupExecutionType.h"
#include "Orders/RTSOrderPreviewData.h"
#include "Orders/RTSOrderProcessPolicy.h"
#include "Orders/RTSOrderResult.h"
#include "Orders/RTSOrderTargetData.h"
#include "Orders/RTSOrderTagRequirements.h"
#include "Orders/RTSTargetType.h"
#include "RTSOrder.generated.h"

class AActor;
class UTexture2D;
struct FRTSOrderErrorTags;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSOrderCallback, ERTSOrderResult, Result);

/**
 * Represents an order type that can be issued to units and buildings.
 */
UCLASS(BlueprintType, Abstract, Blueprintable, Const)
class ORDERSABILITIES_API URTSOrder : public UObject
{
    GENERATED_BODY()

public:
    URTSOrder();

    /** Whether the specified actor can obey this kind of order. */
    virtual bool CanObeyOrder(const AActor* OrderedActor, int32 Index,
                              FRTSOrderErrorTags* OutErrorTags = nullptr) const;

    /** Whether the specified actor and/or location is a valid target for this order. */
    virtual bool IsValidTarget(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
                               FRTSOrderErrorTags* OutErrorTags = nullptr) const;

    /**
     * Creates individual target locations for the group of actors. The default implementation just returns the
     * specified 'TargetLocation' for each actor.
     */
    virtual void CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
                                                 const FRTSOrderTargetData& TargetData,
                                                 TArray<FVector2D>& OutTargetLocations) const;

    /**
     * Issues the specified actor to obey this order on the specified target. The callback must be invoked when the
     * order is finished.
     */
    virtual void IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
                            FRTSOrderCallback Callback, const FVector& HomeLocation) const;

    virtual void OrderCanceled(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const;

    /** Gets the target type this order is using. */
    virtual ERTSTargetType GetTargetType(const AActor* OrderedActor, int32 Index) const;

    /** Whether this order type creates individual target locations for a group of actors */
    virtual bool IsCreatingIndividualTargetLocations(const AActor* OrderedActor, int32 Index) const;

    /** Gets the normal icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetNormalIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the hovered icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetHoveredIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the pressed icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetPressedIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the disabled icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetDisabledIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the Name of the order. Can be shown in the UI. */
    virtual FText GetName(const AActor* OrderedActor, int32 Index) const;

    /** Gets the Description of the order. Can be shown in the UI. */
    virtual FText GetDescription(const AActor* OrderedActor, int32 Index) const;

    /** Gets the index of the button when shown in the UI. */
    virtual int32 GetOrderButtonIndex() const;

    /** Checks whether to use a fixed index of the button when shown in the UI, instead of just lining it up among
     * others. */
    virtual bool HasFixedOrderButtonIndex() const;

    /** Gets details about the preview for the order while choosing a target. */
    virtual FRTSOrderPreviewData GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const;

    /** Gets the tag requirements for this order that must be full filled to be issued. */
    virtual void GetTagRequirements(const AActor* OrderedActor, int32 Index,
                                    FRTSOrderTagRequirements& OutTagRequirements) const;

    /** Gets the tag requirements for this order that must be full filled to be successful. */
    virtual void GetSuccessTagRequirements(const AActor* OrderedActor, int32 Index,
                                           FRTSOrderTagRequirements& OutTagRequirements) const;

    /**
     * Gets the required range between the ordered actor and the target that is needed to fulfill the order.
     * '0' value is returned if the order has no required range.
     */
    virtual float GetRequiredRange(const AActor* OrderedActor, int32 Index) const;

    /** Gets the specific acquisition radius for this order. */
    virtual bool GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index,
                                              float& OutAcquisitionRadius) const;

    /**
     * Gets a value that describes how an order is executed. This might determine how the order is displayed in the UI
     * and it determines how the order is handled by the order system.
     */
    virtual ERTSOrderProcessPolicy GetOrderProcessPolicy(const AActor* OrderedActor, int32 Index) const;

    /** Gets the order to issue instead if the player specified an invalid target for this one. */
    TSoftClassPtr<URTSOrder> GetFallbackOrder() const;

    /**
     * Evaluates the target and returns a score that can be used to compare the different targets. A higher score means
     * a better target for the order.
     */
    virtual float GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const;

    /**
     * Gets the group execution type of this order.
     */
    virtual ERTSOrderGroupExecutionType GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const;

    /** Whether this order is an auto order for the human player. */
    virtual bool IsHumanPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const;

    /** When this order is an auto order, this value indicates whether it is active by default. */
    virtual bool GetHumanPlayerAutoOrderInitialState(const AActor* OrderedActor, int32 Index) const;

    /** Whether this order is an auto order for AI players. */
    virtual bool IsAIPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const;

    /** Whether this order allows auto orders when it is active. */
    virtual bool AreAutoOrdersAllowedDuringOrder() const;

protected:
    /** Tag requirements for an order that must be full filled to be issued. */
    UPROPERTY(Category = "RTS Requirements", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRTSOrderTagRequirements TagRequirements;

    /** Tag requirements for an order that must be full filled to be to be successful. */
    UPROPERTY(Category = "RTS Requirements", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRTSOrderTagRequirements SuccessTagRequirements;

    /**
     * Describes how an order is executed. This might determine how the order is displayed in the UI and it determines
     * how the order is handled by the order system.
     */
    UPROPERTY(Category = "RTS", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    ERTSOrderProcessPolicy OrderProcessPolicy;

private:
    /** Order to issue instead if the player specified an invalid target for this one. */
    UPROPERTY(Category = "RTS", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TSoftClassPtr<URTSOrder> FallbackOrder;
};
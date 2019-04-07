#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"

#include "AttributeSet.h"
#include "Text.h"

#include "Orders/RTSOrderGroupExecutionType.h"
#include "Orders/RTSOrderPreviewData.h"
#include "Orders/RTSOrderTagRequirements.h"
#include "Orders/RTSOrderTargetData.h"
#include "Orders/RTSTargetType.h"

#include "RTSGameplayAbility.generated.h"

// TODO: To support non instanced abilities: The native versions of 'FormatDescription' and 'OnAbilityLevelChanged' need
// the 'Handle', 'ActorInfo' and 'ActivationInfo' as parameter. See 'GetRange'

/**
 * Describes how an ability is executed. This might determine how the ability is displayed in the UI and it determines
 * how the ability is handled by the order system.
 * Note that this has nothing todo with the effects an ability might apply to a target. This has also nothing todo with
 * the cooldown of the ability.
 */
UENUM(BlueprintType)
enum class ERTSAbilityProcessPolicy : uint8
{
    /**
     * The ability has no duration. If it has been activated it is considered to be finished instantly. Note that this
     * does not necessarily mean that 'EndAbility' has already been called. It only means that the order system will not
     * wait until this ability is really finished before it proceeds and it will not actively cancel the ability if
     * another order is issued.
     */
    INSTANT UMETA(DisplayName = "Instant"),

    /**
     * The ability is considered to have a duration. The order system will wait until the ability has been finished
     * ('EndAbility has been called') but will actively cancel the ability when another order is issued.
     */
    CAN_BE_CANCELED UMETA(DisplayName = "Can be Canceled"),

    /**
     * The ability is considered to have a duration. The order system will wait until the ability has been finished
     * ('EndAbility has been called') regardless whether another order has been issued.
     */
    CAN_NOT_BE_CANCELED UMETA(DisplayName = "Can not be Canceled"),

    // clang-format off

    /**
     * Same as 'CanBeCanceled' with the only difference that the ability can not be canceled as long an an ability task
     * is active (e.g. 'PlayMontageAndWaitWithNotify').
     */
    CAN_BE_CANCELED_WHEN_NO_GAMEPLAY_TASK_IS_RUNNING UMETA(DisplayName = "Can be Canceled when no Gameplay Task is running")
    // clang-format on
};

/** Custom game play ability. */
UCLASS(BlueprintType, Abstract, Blueprintable, hideCategories = (Advanced, Input))
class ORDERSABILITIES_API URTSGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    URTSGameplayAbility(const FObjectInitializer& ObjectInitializer);

    /** Gets the target type of this ability. */
    ERTSTargetType GetTargetType() const;

    /** Gets the group execution type this ability. */
    ERTSOrderGroupExecutionType GetGroupExecutionType() const;

    /**
     * Gets the event trigger tag of this ability is triggered with. Note that this just returns the first event tag
     * found in AbilityTriggers.
     */
    UFUNCTION(Category = RTS, BlueprintPure)
    FGameplayTag GetEventTriggerTag() const;

    /** Gets the AbilityTriggerData of this ability. */
    const TArray<FAbilityTriggerData>& GetAbilityTriggerData() const;

    /** Gets the icon of this ability. Can be shown in the UI. */
    UFUNCTION(Category = RTS, BlueprintPure)
    UTexture2D* GetIcon() const;

    /** Gets the name of this ability. Can be shown in the UI. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FText GetName() const;

    /** Gets the description of this ability. Can be shown in the UI. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FText GetDescription(const AActor* Actor) const;

    /** Gets the maximum level of the ability. */
    UFUNCTION(Category = RTS, BlueprintPure)
    int32 GetMaxLevel() const;

    /** Gets details about the preview for this ability while choosing a target. */
    FRTSOrderPreviewData GetAbilityPreviewData() const;

    /** Formats the description by replacing any placeholders by actual values. */
    UFUNCTION(Category = RTS, BlueprintNativeEvent)
    void FormatDescription(const FText& InDescription, const AActor* Actor, FText& OutDescription) const;
    virtual void FormatDescription_Implementation(const FText& InDescription, const AActor* Actor,
                                                  FText& OutDescription) const;

    /** Whether to show this ability as a default order in the UI, instead of as an ability. */
    UFUNCTION(Category = RTS, BlueprintPure)
    bool ShouldShowAsOrderInUI();

    /**
     * Called by the owning gameplay ability system when the level of the owner has been changed. Note that this is not
     * called on non instanced abilities.
     */
    UFUNCTION(Category = RTS, BlueprintNativeEvent)
    void OnAbilityLevelChanged(int32 NewLevel);
    virtual void OnAbilityLevelChanged_Implementation(int32 NewLevel);

    /**
     * Evaluates the target and returns a score that can be used to compare the different targets. A higher score means
     * a better target for the ability.
     */
    UFUNCTION(Category = RTS, BlueprintNativeEvent)
    void GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, float& OutScore) const;
    virtual void GetTargetScore_Implementation(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, float& OutScore) const;

    /** Gets the ability process policy of this ability. */
    ERTSAbilityProcessPolicy GetAbilityProcessPolicy() const;

    /** Gets the tags the activating actor needs to have in order to activate the ability. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FGameplayTagContainer GetActivationRequiredTags() const;

    /** Gets order tag requirements that corresponds with the tags of this ability. */
    void GetOrderTagRequirements(FRTSOrderTagRequirements& OutTagRequirements) const;

    /** Gets source tag requirements of this ability. */
    void GetSourceTagRequirements(FGameplayTagContainer& OutRequiredTags, FGameplayTagContainer& OutBlockedTags) const;

    /** Gets target tag requirements of this ability. */
    void GetTargetTagRequirements(FGameplayTagContainer& OutRequiredTags, FGameplayTagContainer& OutBlockedTags) const;

    /**
     * Gets the minimum range between the caster and the target that is needed to activate the ability.
     * '0' value is returned if the ability has no range.
     */
    UFUNCTION(Category = RTS, BlueprintImplementableEvent, BlueprintPure)
    float GetRange() const;
    virtual float GetRange(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                           const FGameplayAbilityActivationInfo ActivationInfo) const;

    /** Gets the specific acquisition radius for this ability. */
    bool GetAcquisitionRadiusOverride(float& OutAcquisitionRadius) const;

    /** Whether this ability is an auto ability for the human player. */
    bool IsHumanPlayerAutoAbility() const;

    /** When this ability is an auto ability, this value indicates whether it is active by default. */
    bool GetHumanPlayerAutoAutoAbilityInitialState() const;

    /** Whether this ability is an auto ability for AI players. */
    bool IsAIPlayerAutoAbility() const;

    /** Whether this ability uses a specific target score. */
    bool IsTargetScoreOverriden() const;

    /** Whether there are ability tasks active on this gameplay ability instance. */
    bool AreAbilityTasksActive() const;

    //~ Begin UGameplayAbility Interface
    virtual bool ShouldActivateAbility(ENetRole Role) const override;
    virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
    virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
    //~ End UGameplayAbility Interface

protected:
    /**
     * Describes how this ability is executed. This might determine how the ability is displayed in the UI and it
     * determines how the ability is handled by the order system. Note that this has nothing todo with the effects an
     * ability might apply to a target. This has also nothing todo with the cooldown of the ability.
     */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    ERTSAbilityProcessPolicy AbilityProcessPolicy;

    /**
     * To how many and which of the selected units should this order be issued to.
     */
    UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    ERTSOrderGroupExecutionType GroupExecutionType;

    /**
     * The target type of this ability.
     */
    UPROPERTY(Category = "RTS Targeting", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    ERTSTargetType TargetType;

    /** Details about the preview for this ability while choosing a target. */
    UPROPERTY(Category = "RTS Targeting", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FRTSOrderPreviewData AbilityPreviewData;

    /** Icon of the ability. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    UTexture2D* Icon;

    /** Name of the ability. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FText Name;

    /** Description of the ability. Can be shown in the UI. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FText Description;

    /** Maximum level of this ability. */
    UPROPERTY(Category = "RTS Level", BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    int32 MaxLevel;

    /** Whether to show this ability as a default order in the UI, instead of as an ability. */
    UPROPERTY(Category = "RTS Display", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bShouldShowAsOrderInUI;

    /** The specific acquisition radius for this ability. */
    UPROPERTY(Category = "RTS Auto Abilities", EditDefaultsOnly, BlueprintReadOnly,
              meta = (AllowPrivateAccess = true, EditCondition = bIsAcquisitionRadiusOverridden))
    float AcquisitionRadiusOverride;

    /** Whether this ability uses a specific acquisition radius. */
    UPROPERTY(Category = "RTS Auto Abilities", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bIsAcquisitionRadiusOverridden; // InlineEditConditionToggle caused Editor crashes here.

    /** Whether this ability is an auto ability for the human player. */
    UPROPERTY(Category = "RTS Auto Abilities", BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    bool bHumanPlayerAutoAbility;

    /** When this ability is an auto ability, this value indicates whether it is active by default. */
    UPROPERTY(Category = "RTS Auto Abilities", BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bHumanPlayerAutoAutoAbilityInitialState;

    /** Whether this ability is an auto ability for AI players. */
    UPROPERTY(Category = "RTS Auto Abilities", BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    bool bAIPlayerAutoAbility;

    /** Whether this ability uses a specific target score. */
    UPROPERTY(Category = "RTS Auto Abilities", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bIsTargetScoreOverridden;
};

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility.h"
#include "Orders/RTSAutoOrderProvider.h"
#include "Orders/RTSOrderTypeWithIndex.h"
#include "Orders/RTSUseAbilityOrder.h"
#include "AbilitySystem/RTSGameplayAbility.h"
#include "RTSAbilitySystemComponent.generated.h"

class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRTSAbilitySystemComponentCollectedXPChangedSignature, float,
                                             OldCollectedXP, float, NewCollectedXP);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRTSAbilitySystemComponentAbilityPointsChangedSignature, int32,
                                             OldAbilityPoints, int32, NewAbilityPoints);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRTSAbilitySystemComponentAbilityEndedSignature,
                                               TSubclassOf<UGameplayAbility>, Ability, FGameplayAbilitySpecHandle,
                                               AbilitySpecHandle, bool, bWasCancelled);

/** Custom ability system component. */
UCLASS(BlueprintType)
class ORDERSABILITIES_API URTSAbilitySystemComponent : public UAbilitySystemComponent,
                                                 public IRTSAutoOrderProvider
{
    GENERATED_BODY()

public:
    URTSAbilitySystemComponent();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Gets the name of the owner of this component. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FName GetName() const;

    /** Gets the tag that is associated with the owner of this component. */
    FGameplayTag GetNameTag() const;

    /** Sets the level of this actor. This might change the attributes of the actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void SetLevel(int32 NewLevel);

    /** Gets the current level of this actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    int32 GetLevel() const;

    /** Gets the XP this actor grants to its killer. The curve table value depends on the level of this actor.  */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetGrantedXP() const;

    /** Gets the total amount of XP required for reaching the specified level. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetTotalXPRequiredForLevel(int32 InLevel) const;

    /** Gets the amount of XP gained since the last level-up. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetCurrentLevelXP() const;

    /** Gets the total amount of XP required for the next level-up. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetNextLevelXP() const;

    /** Gets the progress of the unit towards reaching the next level [0..1]. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetCurrentLevelXPProgress() const;

    /** Gets the lifetime collected XP of the actor. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetCollectedXP() const;

    /** Adds the specified amount of XP to the actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void AddCollectedXP(float AdditionalCollectedXP);

    /** Whether this actor can level up.  */
    UFUNCTION(Category = RTS, BlueprintPure)
    bool CanLevelUp() const;

    /** Gets the current ability points (granted on level up and required to increase ability levels). */
    UFUNCTION(Category = RTS, BlueprintPure)
    int32 GetAbilityPoints() const;

    /** Sets the current ability points (granted on level up and required to increase ability levels). */
    void SetAbilityPoints(int32 NewAbilityPoints);

    /** Spends an ability point to increase the level of the specified ability. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void IncreaseAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, bool bUseAbilityPoint = true);

    /** Tags that are granted on begin play. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FGameplayTagContainer GetInitialTags() const;

    /**
     * Gets the abilities of this ability system component. This does only include the original abilities and not
     * abilities that might be granted from during gameplay.
     */
    const TArray<TSubclassOf<UGameplayAbility>>& GetAbilities() const;

    /** Gets the abilities of the bought items */
    const TArray<TSubclassOf<UGameplayAbility>>& GetItemAbilities() const;

    /** Getes the abilities the player can unlock during gameplay. */
    const TArray<TSubclassOf<UGameplayAbility>>& GetUnlockableAbilities() const;

    /** Gets the combined list of all initial and unlockable abilities. */
    UFUNCTION(Category = RTS, BlueprintPure)
    TArray<TSubclassOf<UGameplayAbility>> GetInitialAndUnlockableAbilities() const;

    /** Grants the owner the abilities of an item */
    void AddItemAbility(TSubclassOf<UGameplayEffect> GameplayEffectClass);

    /** Removes the owner the ability of an item */
    void RemoveItemAbility(TSubclassOf<UGameplayEffect> GameplayEffectClass);

    /** Adds a tag to this ability system. */
    void AddTag(const FGameplayTag Tag);

    /** Removes a tag from this ability system. */
    void RemoveTag(const FGameplayTag Tag);

    /** Adds the specified tags to this ability system. */
    void AddTags(const FGameplayTagContainer& Tags);

    /** Removes the tags from this ability system. */
    void RemoveTags(const FGameplayTagContainer& Tags);

    /** Event when the lifetime collected XP of the actor have changed. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
    FRTSAbilitySystemComponentCollectedXPChangedSignature OnCollectedXPChanged;

    /** Event when ability points granted on level up and required to increase ability levels have changed. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
    FRTSAbilitySystemComponentAbilityPointsChangedSignature OnAbilityPointsChanged;

    /** Event that is invoked when an ability of this ability system has ended. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
    FRTSAbilitySystemComponentAbilityEndedSignature OnGameplayAbilityEnded;

    /**
     * Gets the minimum range between the caster and the target that is needed to activate the specified ability.
     * '0' value is returned if the ability has no range.
     */
    float GetAbilityRange(TSubclassOf<URTSGameplayAbility> Ability);

    //~ Begin IRTSAutoOrderProvider Interface
    void GetAutoOrders_Implementation(TArray<FRTSOrderTypeWithIndex>& OutAutoOrders);
    //~ End IRTSAutoOrderProvider Interface

protected:
    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    //~ End UActorComponent Interface

    //~ Begin UAbilitySystemComponent Interface
    virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec);
    virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec);
    //~ End UAbilitySystemComponent Interface

private:
    /**
     * Tag that is associated with the owner of this component. This is used to look up the attribute values inside the
     * data tables. In this context the last name of the tag is describing the group inside the curve table.
     *
     * Example:
     * NameTag:             Units.LordHouse.Builder
     * Curve table row key: Builder.Health.MaxHealth
     */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    FGameplayTag NameTag;

    /** Abilities to grant to the unit on begin play. */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<UGameplayAbility>> Abilities;

    /** Abilities the player can unlock later. */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<UGameplayAbility>> UnlockableAbilities;

    /** Initial levels of unlockable abilities the player can level up later. */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    TMap<TSubclassOf<UGameplayAbility>, int32> InitialUnlockableAbilityLevels;

    /** Abilities which are granted via items which can be bought in shops */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true), replicated)
    TArray<TSubclassOf<UGameplayAbility>> ItemAbilities;

    /** Order type that is used to issue a unit to activate an ability. */
    UPROPERTY(Category = "RTS|Orders", BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    TSoftClassPtr<URTSUseAbilityOrder> UseAbilityOrder;

    /**
     * Initial status tags of this unit. These are automatically applied using the attribute sets of that are added to
     * this ability system. They are automatically removed when the owner is killed.
     */
    FGameplayTagContainer InitialStatusTags;

    /**
     * Tags that are granted on begin play.
     */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    FGameplayTagContainer InitialTags;

    /**
     * Tags the are added when the owner dies (the actor might nor be destroyed yet because it is playing its death
     * animation).
     */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    FGameplayTagContainer TagsToAddOnDeath;

    /**
     * Tags the are removed when the owner dies (the actor might nor be destroyed yet because it is playing its death
     * animation).
     */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    FGameplayTagContainer TagsToRemoveOnDeath;

    /** Whether this actor can level up.  */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bCanLevelUp;

    /** Current level of the actor. */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
              meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1, EditCondition = bCanLevelUp))
    int32 Level;

    /** Max level of the hero. */
    UPROPERTY(Category = "RTS|Level", EditInstanceOnly,
              meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1, EditCondition = bCanLevelUp))
    int32 MaxLevel;

    /** Collected XP needed to be on a certain level. */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
              meta = (AllowPrivateAccess = true, EditCondition = bCanLevelUp))
    FScalableFloat XPPerLevel;

    /** Gameplay effect to fire on level up. */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
              meta = (AllowPrivateAccess = true, EditCondition = bCanLevelUp))
    TSubclassOf<UGameplayEffect> LevelUpEffect;

    /** Life time collected XP of the actor. */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, VisibleInstanceOnly, ReplicatedUsing = ReceivedCollectedXP,
              meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1))
    float CollectedXP;

    /** XP this actor grants to its killer. The curve table value depends on the level of this actor.  */
    UPROPERTY(Category = "RTS|Level", BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
    FScalableFloat GrantedXP;

    /** Ability points granted on level up and required to increase ability levels. */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere, ReplicatedUsing = ReceivedAbilityPoints,
              meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1, EditCondition = bCanLevelUp))
    int32 AbilityPoints;

    /** Tags of the player state that controls this actor that should be transfered (copied) to this actor. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, meta = (Categories = Player))
    FGameplayTagContainer TransferPlayerTags;

    /** Tags of the player state that controls this actor that increase ability levels of this actor. */
    UPROPERTY(Category = RTS, EditDefaultsOnly, meta = (Categories = Player))
    TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> IncreaseAbilityLevelPlayerTags;

    /** If changes of the owning tags should be logged. */
    UPROPERTY(Category = RTS, EditDefaultsOnly)
    bool bLogTagChanges;

    /**
     * The handles of the delegates that are registered on the ability system of the player owner to be able to remove
     * them when the player changes.
     */
    TMap<FGameplayTag, FDelegateHandle> RegisteredOwnerTagEventHandles;

    void InitializeAttributes(int AttributeLevel, bool bInitialInit);
    float GetAttributeValueFromCurveTable(const FGameplayAttribute& Attribute, int32 InLevel);

    UFUNCTION()
    void OnKilled(AController* PreviousOwner, AActor* DamageCauser, AActor* KilledUnit);

    UFUNCTION()
    void OnAnyActorKilled(AActor* KilledActor, AController* PreviousOwner, AActor* DamageCauser);

    void ApplyInitialTags();

    UFUNCTION()
    void OnOwnerChanged(APlayerState* PreviousOwner, APlayerState* NewOwner);

    void UnregisterTransferPlayerTags(APlayerState* PlayerOwner);
    void RegisterTransferPlayerTags(APlayerState* PlayerOwner);
    void OnOwningPlayerTagAddedOrRemoved(const FGameplayTag Tag, int32 NewCount);
    void FindAttributeSetStatusTags(FGameplayTagContainer& TagContainer);
    void FindComponentStatusTags(FGameplayTagContainer& TagContainer);

    UFUNCTION()
    void ReceivedCollectedXP(float OldCollectedXP);

    UFUNCTION()
    void ReceivedAbilityPoints(int32 OldAbilityPoints);

    void NotifyOnCollectedXPChanged(float OldCollectedXP, float NewCollectedXP);
    void NotifyOnAbilityPointsChanged(int32 OldAbilityPoints, int32 NewAbilityPoints);

    /** Updates the current level of the actor, based on its current XP value. */
    void UpdateLevel();

    void AbilityEndedCallback(const FAbilityEndedData& AbilityEndedData);
};

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GameplayAbilityTypes.h"
#include "GameplayAbilityTargetTypes.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Text.h"
#include "Orders/RTSOrderTargetData.h"
#include "Orders/RTSTargetType.h"
#include "RTSAbilitySystemHelper.generated.h"


/** Helper function for the ability system. */
UCLASS(BlueprintType)
class ORDERSABILITIES_API URTSAbilitySystemHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ---------------------------------------------------------------------------------------------------
    // Attributes
    // ---------------------------------------------------------------------------------------------------

    /**
     * Gets all game play attributes of all attribute sets (attributes declared in'UAbilitySystemComponent' not
     * included).
     */
    UFUNCTION(Category = RTS, BlueprintPure)
    static const TArray<FGameplayAttribute>& GetGameplayAttributes();

    /**
     * Gets the value of the specified gameplay attribute or the default value if the attribute could not be found.
     * Note that this function does not need to be exposed to blueprints because the function
     * 'UAbilitySystemBlueprintLibrary::GetFloatAttribute' resolves in a more appropriate blueprint node.
     */
    static float GetAttributeValue(const AActor* Actor, const FGameplayAttribute& Attribute,
                                   float DefaultIfNotFound = 0.0f);

    /** Gets the default attribute value with the specified name based on the name tag of the passed actor in the passed
     * table. */
    static float GetAttributeDefaultValue(TSubclassOf<AActor> ActorClass, UCurveTable* CurveTable, FName AttributeName,
                                          float Level = 1.0f);

    // ---------------------------------------------------------------------------------------------------
    // Abilities
    // ---------------------------------------------------------------------------------------------------

    /** Gets all melee or ranged attack ability of the given ability system component. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static TArray<TSubclassOf<UGameplayAbility>> GetBasicAttackAbilities(const UAbilitySystemComponent* AbilitySystem);

    /** Returns the list of all abilities of the given ability system component. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static TArray<TSubclassOf<UGameplayAbility>> GetAbilities(const UAbilitySystemComponent* AbilitySystem);

    /** Returns the list of all abilities of the given ability system component that have the specified ability tags. */
    static TArray<TSubclassOf<UGameplayAbility>> GetAbilitiesByTag(const UAbilitySystemComponent* AbilitySystem,
                                                                   const FGameplayTagContainer& GameplayTagContainer);

    /** Gets the cool down effect class of the specified gameplay ability class. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static TSubclassOf<UGameplayEffect> GetCooldownEffect(TSubclassOf<UGameplayAbility> Ability);

    /** Gets the remaining cool down time. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static float GetCooldownTimeRemaining(const UAbilitySystemComponent* AbilitySystem,
                                          TSubclassOf<UGameplayAbility> Ability);

    /** Gets the cool down duration. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static float GetCooldownTimeDuration(const UAbilitySystemComponent* AbilitySystem,
                                         TSubclassOf<UGameplayAbility> Ability);

    /** Gets the cool down time and duration. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static void GetCooldownTimeRemainingAndDuration(const UAbilitySystemComponent* AbilitySystem,
                                                    TSubclassOf<UGameplayAbility> Ability,
                                                    float& OutRemainingCooldownTime, float& OutCooldownDuration);

    /** Gets the icon of the specified gameplay ability class. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static UTexture2D* GetAbilityIcon(TSubclassOf<UGameplayAbility> Ability);

    /** Gets the current level of an ability. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static int32 GetAbilityLevel(TSubclassOf<UGameplayAbility> Ability, AActor* Actor);

    /** Gets the maximum level of the ability. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
    static int32 GetAbilityMaxLevel(UObject* WorldContextObject, TSubclassOf<UGameplayAbility> Ability);

    /** Gets the hero level required to increase the level of the specified ability. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure, meta = (WorldContext = "WorldContextObject"))
    static int32 GetRequiredLevelToIncreaseAbilityLevel(UObject* WorldContextObject, AActor* Actor, TSubclassOf<UGameplayAbility> Ability);

    /** Gets the name of the ability. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static FText GetAbilityName(TSubclassOf<UGameplayAbility> Ability);

    /** Gets the name of the ability. Can be shown in the UI. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static FText GetAbilityDescription(TSubclassOf<UGameplayAbility> Ability, AActor* Actor);

    /** Gets the target type of the specified gameplay ability class. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static ERTSTargetType GetAbilityTargetType(TSubclassOf<UGameplayAbility> Ability);

    /** Whether to show abilities of the specified class as default orders in the UI, instead of as abilities. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static bool ShouldShowAbilityAsOrderInUI(TSubclassOf<UGameplayAbility> Ability);

    /** Cancels the ability of the specified type.  */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintCallable)
    static void CancelAbility(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability);

    /** Whether the specified ability is currently active. Returns always false for clients.  */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure, meta = (DisplayName = "IsAbilityActive (Server only)"))
    static bool IsAbilityActive(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability);

    /** Whether the specified passive ability is unlocked and active.  */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static bool IsPassiveAbilityUnlocked(const UAbilitySystemComponent* AbilitySystem,
                                         TSubclassOf<UGameplayAbility> Ability);

    /**
     * Gets the minimum range between the caster and the target that is needed to activate the specified ability.
     * '0' value is returned if the ability has no range.
     */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static float GetAbilityRange(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability);

    /** Gets the ability process policy of the specified ability. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static ERTSAbilityProcessPolicy GetAbilityProcessPolicy(TSubclassOf<UGameplayAbility> Ability);

    /** Gets source tag requirements of the specified ability. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static void GetAbilitySourceTagRequirements(TSubclassOf<UGameplayAbility> Ability,
                                                FGameplayTagContainer& OutRequiredTags,
                                                FGameplayTagContainer& OutBlockedTags);

    /** Gets target tag requirements of the specified ability. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static void GetAbilityTargetTagRequirements(TSubclassOf<UGameplayAbility> Ability,
                                                FGameplayTagContainer& OutRequiredTags,
                                                FGameplayTagContainer& OutBlockedTags);

    /** Whether the ability is an auto ability for the human player. */
    UFUNCTION(Category = "RTS Ability|Abilities", BlueprintPure)
    static bool IsHumanPlayerAutoAbility(TSubclassOf<UGameplayAbility> Ability);

    // ---------------------------------------------------------------------------------------------------
    // Effects
    // ---------------------------------------------------------------------------------------------------

    /**
     * Gets the remaining time of the specified effect.  Returns 0 if the effect is not active on this ability
     * system.
     */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static float GetActiveEffectTimeRemaining(const UAbilitySystemComponent* AbilitySystem,
                                              TSubclassOf<UGameplayEffect> Effect);

    /**
     * Gets the duration of the gameplay effect that is currently active on the ability system component. Returns 0 if
     * the effect is not active on this ability system.
     */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static float GetActiveEffectDuration(const UAbilitySystemComponent* AbilitySystem,
                                         TSubclassOf<UGameplayEffect> Effect);

    /** Gets the cool down time and duration. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static void GetActiveEffectTimeRemainingAndDuration(const UAbilitySystemComponent* AbilitySystem,
                                                        TSubclassOf<UGameplayEffect> Effect, float& OutRemainingTime,
                                                        float& OutDuration);

    /** Returns the list of all gameplay effects that are currently active for the given ability system component. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static TArray<TSubclassOf<UGameplayEffect>> GetActiveGameplayEffects(const UAbilitySystemComponent* AbilitySystem);

    /** Returns the list of all gameplay effects that are currently active for the given ability system component, along
     * with additional data such as duration and stacks. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static TArray<FGameplayEffectSpec> GetActiveGameplayEffectSpecs(const UAbilitySystemComponent* AbilitySystem);

    /** Returns the gameplay effect class of the specified gameplay effect spec. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static TSubclassOf<UGameplayEffect> GetGameplayEffectSpecClass(const FGameplayEffectSpec& GameplayEffectSpec);

    /** Returns the number of stacks of the specified gameplay effect spec. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static int32 GetGameplayEffectSpecStackCount(const FGameplayEffectSpec& GameplayEffectSpec);

    /** Gets the icon of the specified gameplay effect class. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static UTexture2D* GetEffectIcon(TSubclassOf<UGameplayEffect> Effect);

    /** Gets the name of the specified gameplay effect class. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static FText GetEffectName(TSubclassOf<UGameplayEffect> Effect);

    /** Gets the description of the specified gameplay effect class. */
    UFUNCTION(Category = "RTS Ability|Effects", BlueprintPure)
    static FText GetEffectDescription(TSubclassOf<UGameplayEffect> Effect);

    // ---------------------------------------------------------------------------------------------------
    // Tags
    // ---------------------------------------------------------------------------------------------------

    /** Gets the last single name of the specified Tag. */
    UFUNCTION(BlueprintPure, Category = "RTS")
    static FName GetLastTagName(FGameplayTag Tag);

    /** Gets all gameplay tags from the specified ability system. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static FGameplayTagContainer GetGameplayTags(const UAbilitySystemComponent* AbilitySystem);

    /** Gets the tag count of the specified  gameplay tag from the specified ability system. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static int32 GetGameplayTagCount(const UAbilitySystemComponent* AbilitySystem, FGameplayTag TagToCheck);

    /** Whether the specified ability system has the specified tag. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasMatchingGameplayTag(const UAbilitySystemComponent* AbilitySystem, FGameplayTag TagToCheck);

    /** Whether the specified ability system has all specified tags. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasAllMatchingGameplayTags(const UAbilitySystemComponent* AbilitySystem,
                                           const FGameplayTagContainer& TagContainer);

    /** Whether the specified ability system has any of the specified tags. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasAnyMatchingGameplayTags(const UAbilitySystemComponent* AbilitySystem,
                                           const FGameplayTagContainer& TagContainer);

    /** Whether the specified actor has the specified tag. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasActorMatchingGameplayTag(const AActor* Actor, FGameplayTag TagToCheck);

    /** Whether the specified actor has the specified tag. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasActorClassMatchingGameplayTag(TSubclassOf<AActor> ActorClass, FGameplayTag TagToCheck);

    /** Whether the specified actor has all specified tags. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasActorAllMatchingGameplayTags(const AActor* Actor, const FGameplayTagContainer& TagContainer);

    /** Whether the specified actor has any of the specified tags. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool HasActorAnyMatchingGameplayTags(const AActor* Actor, const FGameplayTagContainer& TagContainer);

    /** Gets the gameplay tags of the specified actor. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static void GetTags(const AActor* Actor, FGameplayTagContainer& OutGameplayTags);

    /** Gets the gameplay tags of the player owner of the specified actor. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static void GetPlayerOwnerTags(const AActor* Actor, FGameplayTagContainer& OutGameplayTags);

    /**
     * Gets the tags describing the relationship of the first actor to the other (friendly, hostile, neutral, same
     * player, visibility.
     */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static FGameplayTagContainer GetRelationshipTags(const AActor* Actor, const AActor* Other);

    // NOTE(np): In A Year Of Rain, we're adding relationship tags based on the team assignments of both players.
    ///**
    // * Gets the tags describing the relationship of the first player to the other (friendly, hostile, neutral, same
    // * player, visibility).
    // */
    //UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    //static void GetRelationshipTagsFromPlayers(const ARTSPlayerState* ActorPlayerState,
    //                                           const ARTSPlayerState* OtherPlayerState,
    //                                           FGameplayTagContainer& OutRelationshipTags);
    /**
     * Gets the gameplay tags of the specified actors, including relationship tags.
     */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static void GetSourceAndTargetTags(const AActor* SourceActor, const AActor* TargetActor,
                                       FGameplayTagContainer& OutSourceTags, FGameplayTagContainer& OutTargetTags);

    /** Creates a gameplay tag container that has all tags that are present in the specified container.*/
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static FGameplayTagContainer UnionGameplayTagContainers(const FGameplayTagContainer& FirstTagContainer,
                                                            const FGameplayTagContainer& SecondTagContainer);

    /**
     * Changes the state of a single gameplay tag on the specified actor. If 'bExistence' is true the tag is added if it
     * does not already exist. If 'bExistence' is false the tag is removed if it exists.
     */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintCallable)
    static void ChangeTag(AActor* Actor, const FGameplayTag& Tag, bool bExistence);

    /** Checks if the specified tags has all of the specified required tags and none of the specified blocked tags. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool DoesSatisfyTagRequirements(const FGameplayTagContainer& Tags, const FGameplayTagContainer& RequiredTags,
                                           const FGameplayTagContainer& BlockedTags);

    /** Checks if the specified tags has all of the specified required tags and none of the specified blocked tags. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool DoesSatisfyTagRequirementsWithResult(const FGameplayTagContainer& Tags,
                                                     const FGameplayTagContainer& InRequiredTags,
                                                     const FGameplayTagContainer& InBlockedTags,
                                                     FGameplayTagContainer& OutMissingTags,
                                                     FGameplayTagContainer& OutBlockingTags);

    /**
     * Returns a tag container with all tags from the specified tag container that are derived from the specified
     * parent tag.
     */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintCallable)
    static FGameplayTagContainer FilterForTagsWithParentTag(const FGameplayTagContainer& TagContainer,
                                                            FGameplayTag ParentTag);
    // ---------------------------------------------------------------------------------------------------
    // Gameplay Events
    // ---------------------------------------------------------------------------------------------------

    /** Fills a 'FGameplayEventData' with values from the specified ability and order target data. */
    UFUNCTION(Category = "RTS Ability|Event", BlueprintPure)
    static void CreateGameplayEventData(AActor* Source, const FRTSOrderTargetData& TargetData,
                                        TSubclassOf<UGameplayAbility> Ability, FGameplayEventData& OutEventData);

    /** Fills a 'FGameplayEventData' with values from the specified Event-Tag without EventData. */
    UFUNCTION(Category = "RTS Ability|Event", BlueprintPure)
    static void CreateGameplayEventDataWithEventTag(AActor* Source, AActor* Target, FGameplayTag EventTag,
                                                    FGameplayEventData& OutEventData);

    /**
     * Sends a gameplay event to the specified actor. Returns the number of successful ability activations triggered by
     * the event.
     */
    UFUNCTION(Category = "RTS Ability|Event", BlueprintCallable)
    static int32 SendGameplayEvent(AActor* Actor, FGameplayEventData Payload);

    // ---------------------------------------------------------------------------------------------------
    // Curve Tables
    // ---------------------------------------------------------------------------------------------------

    /**
     * Gets the value at the specified level of the specified scalable float or the constant value if no curve table
     * row has been specified.
     */
    UFUNCTION(Category = "RTS Ability|Curve Tables", BlueprintPure)
    static float GetValueAtLevel(const FScalableFloat& ScalableFloat, float Level);

    // ---------------------------------------------------------------------------------------------------
    // Helper
    // ---------------------------------------------------------------------------------------------------

    /** Searches the components attached to the specified actor class and returns the first encountered component of the
     * specified class. */
    template <class T>
    static T* FindDefaultComponentByClass(const TSubclassOf<AActor> InActorClass)
    {
        return (T*)FindDefaultComponentByClass(InActorClass, T::StaticClass());
    }

    /** Searches the components attached to the specified actor class and returns the first encountered component of the
     * specified class. */
    UFUNCTION(BlueprintCallable, Category = "RTS")
    static UActorComponent* FindDefaultComponentByClass(const TSubclassOf<AActor> InActorClass,
                                                        const TSubclassOf<UActorComponent> InComponentClass);

    /** Whether 'Other' is visible for 'Actor'. */
    UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    static bool IsVisibleForActor(const AActor* Actor, const AActor* Other);

    UFUNCTION(Category = "RTS Ability|Gameplay Effects", BlueprintPure)
    static TArray<TSubclassOf<UGameplayAbility>>
    GetGrantedAbilitiesFromGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect);

    /** Gets the index of the ItemAbilities Array in the GameplayAbilitySystem */
    UFUNCTION(Category = "RTS Ability|Gameplay Abilities", BlueprintPure)
    static bool GetItemAbilityIndex(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> AbilityClass,
                                    int32& OutIndex);

    /** Creates an ability target data from the specified order target data. */
    static FGameplayAbilityTargetDataHandle
    CreateAbilityTargetDataFromOrderTargetData(AActor* OrderedActor, const FRTSOrderTargetData& OrderTargetData,
                                               ERTSTargetType TargetType);

    // ---------------------------------------------------------------------------------------------------
    // GameplayCue
    // ---------------------------------------------------------------------------------------------------

    /**
     * Executes GameplayCues without being attached to a GameplayAbilityObject.
     */
    UFUNCTION(Category = "RTS Ability|GameplayCue", BlueprintCallable)
    static void ExecuteGameplayCueWithParamsUnattached(AActor* Actor, FGameplayTag AreaOfEffectGameplayCue,
                                                       const FGameplayCueParameters& GameplayCueParameters);

private:
    static TArray<FGameplayAttribute> FindGameplayAttributes();
};

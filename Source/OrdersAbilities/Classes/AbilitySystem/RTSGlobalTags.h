#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RTSGlobalTags.generated.h"

struct FGameplayTag;

/** Global tags that need to be exposed to C++. */
// TODO: Load tag names from config file?
UCLASS()
class ORDERSABILITIES_API URTSGlobalTags : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ---------------------------------------------------------------------------------------------------
    // Permanent status tags
    // ---------------------------------------------------------------------------------------------------

    static const FGameplayTag& Status_Permanent();

    /** Whether the actor is generally able to attack. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_CanAttack();

    /** Whether the actor is generally able to repair. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_CanRepair();

    /** Whether the actor is generally able to to construct buildings. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_CanConstruct();

    /** Whether the actor is generally able to to produce actors. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_CanProduce();

    /** Whether the actor is generally movable. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_Movable();

    /** Whether the actor is generally able to act as container. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_IsContainer();

    /** Whether the actor can be contained by another actor. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_IsContainable();

    /** Whether the actor is generally able to gather resources. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_CanGather();

    /** Whether the actor is generally able to provide resources for gatherers. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_IsResourceSource();

    /** Whether the actor is generally able to accept resources of gatherers. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_IsResourceDrain();

    /** Whether the actor is a summoned unit. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_Summoned();

    /** Whether the actor has an inventory. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_HasInventory();

    /** Whether the actor is an item. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_IsItem();

    /** Whether the actor buys and sells items. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_IsShop();

    // ---------------------------------------------------------------------------------------------------
    // Changing status tags
    // ---------------------------------------------------------------------------------------------------

    static const FGameplayTag& Status_Changing();

    /** Whether the actor is still alive. This is only changed when the actor dies. */
    static const FGameplayTag& Status_Changing_IsAlive();

    /** Whether the actor is currently not able to attack. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Unarmed();

    /** Whether the actor is currently not able to use abilities. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Silenced();

    /** Whether the actor is currently hidden from most enemies. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Stealthed();

    /** Whether the actor is currently able to detect stealthed units. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Detector();

    /** Whether the actor is currently not able to move. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Immobilized();

    /** Whether the actor is currently moving. */
    static const FGameplayTag& Status_Changing_IsMoving();

    /** Whether the actor is currently invulnerable. */
    static const FGameplayTag& Status_Changing_Invulnerable();

    /** Whether the actor is currently invisible. This is different to stealth because stealth can be detected. */
    static const FGameplayTag& Status_Changing_Invisible();

    /** Whether the actors current health is less than its max health. */
    static const FGameplayTag& Status_Changing_Injured();

    /** Whether the actor is currently under construction. */
    static const FGameplayTag& Status_Changing_UnderConstruction();

    /** Whether the actor is currently constructing another actor. */
    static const FGameplayTag& Status_Changing_Constructing();

    /** Whether any actor can enter the container without a special order (e.g. without harvesting or constructing).
     * This might change during gameplay. */
    static const FGameplayTag& Status_Changing_ContainerCanLoadAnyone();

    /** Whether the capacity of the container is reached. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_ContainerCapacityReached();

    /** Whether the actor that acts as gatherer has reached its gather capacity. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_GatherCapacityReached(); // TODO: Currently not set by gather component

    /** Whether the actor that acts as gatherer is currently carrying resources. Check the 'Resource' tag to find out
     * which type of the resource. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_IsCarryingResources();

    /** Whether the actor can lose life, but can't die and the lowest health will be clamped on 1.0f. This might change
     * during gameplay. */
    static const FGameplayTag& Status_Changing_LastStand();

    /** Whether the actor is sleeping (stun), until he gets damage */
    static const FGameplayTag& Status_Changing_Sleeped();

    /** Whether the actor will not get any damage, but don't cancel the attackingbehavior of enemies like
     * "Invulnerable". (Useful for "absorb" abilities)*/
    static const FGameplayTag& Status_Changing_DamageAbsorbing();

    // ---------------------------------------------------------------------------------------------------
    // Relationship tags
    // ---------------------------------------------------------------------------------------------------

    /** Relationship parent tag. */
    static const FGameplayTag& Relationship();

    /** Whether the two actor references resolve to the same actor. */
    static const FGameplayTag& Relationship_Self();

    /** Whether the relationship to this actor is friendly. */
    static const FGameplayTag& Relationship_Friendly();

    /** Whether the relationship to this actor is hostile. */
    static const FGameplayTag& Relationship_Hostile();

    /** Whether the relationship to this actor is neutral. */
    static const FGameplayTag& Relationship_Neutral();

    /** Whether this actor is controlled by the same player. */
    static const FGameplayTag& Relationship_SamePlayer();

    /** Whether the actor is visible. */
    static const FGameplayTag& Relationship_Visible();

    // ---------------------------------------------------------------------------------------------------
    // Name tags
    // ---------------------------------------------------------------------------------------------------

    /** Whether this actor is considered to be a building. */
    static const FGameplayTag& Building();

    /** Whether this actor is considered to be a unit. */
    static const FGameplayTag& Unit();

    /** Whether this actor is considered to be a hero. */
    static const FGameplayTag& Hero();

    // ---------------------------------------------------------------------------------------------------
    // Resource tags
    // ---------------------------------------------------------------------------------------------------

    /** The resource type gold. */
    static const FGameplayTag& Resource_Gold();

    /** The resource type lumber. */
    static const FGameplayTag& Resource_Lumber();

    // ---------------------------------------------------------------------------------------------------
    // Ability activation failure tags
    // ---------------------------------------------------------------------------------------------------

    /** The ability cannot be activated because of a blocking tag. */
    static const FGameplayTag& AbilityActivationFailure_BlockedTag();

    /** The ability cannot be activated because of a required tag. */
    static const FGameplayTag& AbilityActivationFailure_MissingTag();

    /** The ability cannot be activated because it is still on cooldown. */
    static const FGameplayTag& AbilityActivationFailure_Cooldown();

    /** The ability cannot be activated because the required costs cannot be payed. */
    static const FGameplayTag& AbilityActivationFailure_Cost();

    /** The ability cannot be activated because no target has been specified. */
    static const FGameplayTag& AbilityActivationFailure_NoTarget();

    // ---------------------------------------------------------------------------------------------------
    // Event tags
    // ---------------------------------------------------------------------------------------------------

    /** Whether will trigger an additional GameplayAbility, which will send by the RTSMeeleAttackAbility and
     * RTSRangedAttackAbility */
    static const FGameplayTag& Event_OnHitEffect();

    /** Whether tag the GameplayAbility as a BasicAttack, if it used as a trigger-event-tag (This event will be called
     * for every attack-command). Also it is an indicator for the commandgrid and will hide those abilities */
    static const FGameplayTag& Event_Attack();

    // ---------------------------------------------------------------------------------------------------
    // Ability tags
    // ---------------------------------------------------------------------------------------------------

    /** Whether set the ability in the OnHit-Category and will be triggered by melee- and range-hits */
    static const FGameplayTag& Ability_OnHitEffect();

    /** Whether defines the GameplayEffect as an Attack-Cooldown-Effect and will be removed when a AutoHit will be
     * canceled before the AttackEffect is applied  */
    static const FGameplayTag& Ability_Attack();

    /** Whether defines the GameplayEffect as a Sleep-Ability. The effect will be removed after the units received
     * damage */
    static const FGameplayTag& Ability_Sleep();

    // ---------------------------------------------------------------------------------------------------
    // Classification tags
    // ---------------------------------------------------------------------------------------------------

    /** Whether the actor is a melee-unit */
    static const FGameplayTag& Classification_Melee();

    /** Whether the actor is a ranged-unit */
    static const FGameplayTag& Classification_Ranged();

    // ---------------------------------------------------------------------------------------------------
    // Container tags
    // ---------------------------------------------------------------------------------------------------

    /** Container for constructing buildings. */
    static const FGameplayTag& Container_ConstructionSite();

    /** Container for gathering resources from. */
    static const FGameplayTag& Container_ResourceSource();
};

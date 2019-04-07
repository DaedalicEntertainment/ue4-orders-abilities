#include "AbilitySystem/RTSGlobalTags.h"

#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"


// ---------------------------------------------------------------------------------------------------
// Permanent status tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Status_Permanent()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_CanAttack()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.CanAttack")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_CanRepair()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.CanRepair")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_CanConstruct()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.CanConstruct")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_CanProduce()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.CanProduce")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_Movable()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.Movable")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_IsContainer()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.IsContainer")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_IsContainable()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.IsContainable")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_CanGather()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.CanGather")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_IsResourceSource()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.IsResourceSource")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_IsResourceDrain()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.IsResourceDrain")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_Summoned()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.Summoned")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_HasInventory()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.HasInventory")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_IsItem()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.IsItem")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Permanent_IsShop()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Permanent.IsShop")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Changing status tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Status_Changing()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_IsAlive()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.IsAlive")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Immobilized()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Immobilized")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Unarmed()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Unarmed")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Silenced()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Silenced")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Stealthed()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Stealthed")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Detector()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Detector")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_IsMoving()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.IsMoving")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Invulnerable()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Invulnerable")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Invisible()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Invisible")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Injured()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Injured")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_UnderConstruction()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.UnderConstruction")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Constructing()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Constructing")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_ContainerCanLoadAnyone()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.ContainerCanLoadAnyone")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_ContainerCapacityReached()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.ContainerCapacityReached")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_GatherCapacityReached()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.GatherCapacityReached")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_IsCarryingResources()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.IsCarryingResources")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_LastStand()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.LastStand")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_Sleeped()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.Sleeped")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Status_Changing_DamageAbsorbing()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.Changing.DamageAbsorbing")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Relationship tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Relationship()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Relationship_Self()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Self")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Relationship_Friendly()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Friendly")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Relationship_Hostile()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Hostile")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Relationship_Neutral()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Neutral")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Relationship_SamePlayer()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.SamePlayer")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Relationship_Visible()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Visible")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Name tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Building()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Building")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Unit()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Unit")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Hero()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Hero")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Resource tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Resource_Gold()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Resource.Gold")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Resource_Lumber()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Resource.Lumber")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Ability activation failure tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::AbilityActivationFailure_BlockedTag()
{
    return UAbilitySystemGlobals::Get().ActivateFailTagsBlockedTag;
}

const FGameplayTag& URTSGlobalTags::AbilityActivationFailure_MissingTag()
{
    return UAbilitySystemGlobals::Get().ActivateFailTagsMissingTag;
}

const FGameplayTag& URTSGlobalTags::AbilityActivationFailure_Cooldown()
{
    return UAbilitySystemGlobals::Get().ActivateFailCooldownTag;
}

const FGameplayTag& URTSGlobalTags::AbilityActivationFailure_Cost()
{
    return UAbilitySystemGlobals::Get().ActivateFailCostTag;
}

const FGameplayTag& URTSGlobalTags::AbilityActivationFailure_NoTarget()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("AbilityActivationFailure.NoTarget")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Event tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Event_OnHitEffect()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Event.OnHitEffect")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Event_Attack()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Event.Attack")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Ability tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Ability_OnHitEffect()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.OnHitEffect")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Ability_Attack()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Attack")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Ability_Sleep()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Sleep")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Classification tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Classification_Melee()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Classification.Melee")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Classification_Ranged()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Classification.Ranged")));
    return Tag;
}

// ---------------------------------------------------------------------------------------------------
// Container tags
// ---------------------------------------------------------------------------------------------------

const FGameplayTag& URTSGlobalTags::Container_ConstructionSite()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Container.ConstructionSite")));
    return Tag;
}

const FGameplayTag& URTSGlobalTags::Container_ResourceSource()
{
    static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Container.ResourceSource")));
    return Tag;
}

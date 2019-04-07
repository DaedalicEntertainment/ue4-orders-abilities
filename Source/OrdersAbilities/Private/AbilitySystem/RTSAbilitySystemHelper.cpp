#include "AbilitySystem/RTSAbilitySystemHelper.h"

#include "OrdersAbilities.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "GameplayCueManager.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "UnrealType.h"
#include "UObjectIterator.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Kismet/DataTableFunctionLibrary.h"

#include "AbilitySystem/RTSAbilitySystemComponent.h"
#include "AbilitySystem/RTSGameplayAbility.h"
#include "AbilitySystem/RTSGameplayEffect.h"
#include "AbilitySystem/RTSGlobalTags.h"
#include "Orders/RTSOrderTargetData.h"


// ---------------------------------------------------------------------------------------------------
// Attributes
// ---------------------------------------------------------------------------------------------------

const TArray<FGameplayAttribute>& URTSAbilitySystemHelper::GetGameplayAttributes()
{
    static TArray<FGameplayAttribute> Attributes = FindGameplayAttributes();
    return Attributes;
}

float URTSAbilitySystemHelper::GetAttributeValue(const AActor* Actor, const FGameplayAttribute& Attribute,
                                                 float DefaultIfNotFound /*= 0.0f*/)
{
    bool bFound = false;
    float Value = UAbilitySystemBlueprintLibrary::GetFloatAttribute(Actor, Attribute, bFound);

    if (bFound)
    {
        return Value;
    }

    return DefaultIfNotFound;
}

float URTSAbilitySystemHelper::GetAttributeDefaultValue(TSubclassOf<AActor> ActorClass, UCurveTable* CurveTable,
                                                        FName AttributeName, float Level /*= 1.0f*/)
{
    URTSAbilitySystemComponent* AbilitySystemComponent =
        FindDefaultComponentByClass<URTSAbilitySystemComponent>(ActorClass);

    if (!IsValid(AbilitySystemComponent))
    {
        return 0.0f;
    }

    FString NameTag = GetLastTagName(AbilitySystemComponent->GetNameTag()).ToString();
    FString AttributeTag = AttributeName.ToString();
    FString RowName = FString::Format(TEXT("{0}.{1}"), {NameTag, AttributeTag});

    TEnumAsByte<EEvaluateCurveTableResult::Type> Result;
    float AttributeValue;
    FString ContextString;

    UDataTableFunctionLibrary::EvaluateCurveTableRow(CurveTable, FName(*RowName), Level, Result, AttributeValue,
                                                     ContextString);
    return Result == EEvaluateCurveTableResult::Type::RowFound ? AttributeValue : 0.0f;
}

TArray<TSubclassOf<UGameplayAbility>>
URTSAbilitySystemHelper::GetBasicAttackAbilities(const UAbilitySystemComponent* AbilitySystem)
{
    TArray<TSubclassOf<UGameplayAbility>> Abilities = GetAbilities(AbilitySystem);

    TArray<TSubclassOf<UGameplayAbility>> BasicAttackAbilities;

    URTSGameplayAbility* GameplayAbility;

    for (TSubclassOf<UGameplayAbility> Ability : Abilities)
    {
        if (Ability == nullptr)
        {
            continue;
        }

        GameplayAbility = Ability->GetDefaultObject<URTSGameplayAbility>();

        if (!IsValid(GameplayAbility))
        {
            continue;
        }

        if (GameplayAbility->GetEventTriggerTag() == URTSGlobalTags::Event_Attack())
        {
            BasicAttackAbilities.Add(Ability);
        }
    }

    return BasicAttackAbilities;
}

// ---------------------------------------------------------------------------------------------------
// Abilities
// ---------------------------------------------------------------------------------------------------

TArray<TSubclassOf<UGameplayAbility>>
URTSAbilitySystemHelper::GetAbilities(const UAbilitySystemComponent* AbilitySystem)
{
    TArray<TSubclassOf<UGameplayAbility>> OutAbilities;
    if (AbilitySystem == nullptr)
    {
        return OutAbilities;
    }

    for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystem->GetActivatableAbilities())
    {
        OutAbilities.Add(AbilitySpec.Ability->GetClass());
    }

    return OutAbilities;
}

TArray<TSubclassOf<UGameplayAbility>>
URTSAbilitySystemHelper::GetAbilitiesByTag(const UAbilitySystemComponent* AbilitySystem,
                                           const FGameplayTagContainer& GameplayTagContainer)
{
    TArray<TSubclassOf<UGameplayAbility>> OutAbilities;
    if (AbilitySystem == nullptr)
    {
        return OutAbilities;
    }

    TArray<FGameplayAbilitySpec*> Abilities;
    AbilitySystem->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, Abilities);

    for (FGameplayAbilitySpec* GameplayAbilitySpec : Abilities)
    {
        OutAbilities.Add(GameplayAbilitySpec->Ability->GetClass());
    }

    return OutAbilities;
}

TSubclassOf<UGameplayEffect> URTSAbilitySystemHelper::GetCooldownEffect(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return nullptr;
    }

    UGameplayEffect* CooldownEffect = Ability->GetDefaultObject<UGameplayAbility>()->GetCooldownGameplayEffect();
    return CooldownEffect != nullptr ? CooldownEffect->GetClass() : nullptr;
}

float URTSAbilitySystemHelper::GetCooldownTimeRemaining(const UAbilitySystemComponent* AbilitySystem,
                                                        TSubclassOf<UGameplayAbility> Ability)
{
    float Time;
    float Duration;
    GetCooldownTimeRemainingAndDuration(AbilitySystem, Ability, Time, Duration);
    return Time;
}

float URTSAbilitySystemHelper::GetCooldownTimeDuration(const UAbilitySystemComponent* AbilitySystem,
                                                       TSubclassOf<UGameplayAbility> Ability)
{
    float Time;
    float Duration;
    GetCooldownTimeRemainingAndDuration(AbilitySystem, Ability, Time, Duration);
    return Duration;
}

void URTSAbilitySystemHelper::GetCooldownTimeRemainingAndDuration(const UAbilitySystemComponent* AbilitySystem,
                                                                  TSubclassOf<UGameplayAbility> Ability,
                                                                  float& OutRemainingCooldownTime,
                                                                  float& OutCooldownDuration)
{
    TSubclassOf<UGameplayEffect> CooldownEffectClass = GetCooldownEffect(Ability);

    GetActiveEffectTimeRemainingAndDuration(AbilitySystem, CooldownEffectClass, OutRemainingCooldownTime,
                                            OutCooldownDuration);

    if (CooldownEffectClass && OutCooldownDuration == 0)
    {
        // If we didn't find any cooldown duration, it might be due to the fact that no cooldown effect is active. Try
        // using the CDO.
        const UGameplayEffect* CooldownEffect = CooldownEffectClass->GetDefaultObject<UGameplayEffect>();
        FGameplayEffectSpec CooldownEffectSpec(CooldownEffect, FGameplayEffectContextHandle());
        CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(CooldownEffectSpec, OutCooldownDuration);
    }
}

UTexture2D* URTSAbilitySystemHelper::GetAbilityIcon(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return nullptr;
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    return GameplayAbility != nullptr ? GameplayAbility->GetIcon() : nullptr;
}

int32 URTSAbilitySystemHelper::GetAbilityLevel(TSubclassOf<UGameplayAbility> Ability, AActor* Actor)
{
    if (!IsValid(Actor))
    {
        return 0;
    }

    const URTSAbilitySystemComponent* AbilitySystem = Actor->FindComponentByClass<URTSAbilitySystemComponent>();

    const TArray<FGameplayAbilitySpec>& AbilitySpecs = AbilitySystem->GetActivatableAbilities();
    for (const FGameplayAbilitySpec& Spec : AbilitySpecs)
    {
        if (Spec.Ability->GetClass() == Ability)
        {
            return Spec.Level;
        }
    }

    return 0;
}

int32 URTSAbilitySystemHelper::GetAbilityMaxLevel(UObject* WorldContextObject, TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return 0;
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());

    if (GameplayAbility == nullptr)
    {
        return 0;
    }

    int32 MaxLevel = GameplayAbility->GetMaxLevel();

    if (MaxLevel > 0)
    {
        return MaxLevel;
    }

    return 0;
}

int32 URTSAbilitySystemHelper::GetRequiredLevelToIncreaseAbilityLevel(UObject* WorldContextObject, AActor* Actor, TSubclassOf<UGameplayAbility> Ability)
{
    return GetAbilityLevel(Ability, Actor) * 2 + 1;
}

FText URTSAbilitySystemHelper::GetAbilityName(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return FText::FromString(
            TEXT("URTSAbilitySystemHelper::GetAbilityName: Error: Parameter 'Ability' was 'nullptr'."));
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    if (GameplayAbility == nullptr)
    {
        return FText::FromString(TEXT(
            "URTSAbilitySystemHelper::GetAbilityName: Error: Parameter 'Ability' was not a 'RTSGameplayAbility'."));
    }

    return GameplayAbility->GetName();
}

FText URTSAbilitySystemHelper::GetAbilityDescription(TSubclassOf<UGameplayAbility> Ability, AActor* Actor)
{
    if (Ability == nullptr)
    {
        return FText::FromString(
            TEXT("URTSAbilitySystemHelper::GetAbilityDescription: Error: Parameter 'Ability' was 'nullptr'."));
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    if (GameplayAbility == nullptr)
    {
        return FText::FromString(TEXT("URTSAbilitySystemHelper::GetAbilityDescription: Error: Parameter 'Ability' was "
                                      "not a 'RTSGameplayAbility'."));
    }

    return GameplayAbility->GetDescription(Actor);
}

ERTSTargetType URTSAbilitySystemHelper::GetAbilityTargetType(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return ERTSTargetType::NONE;
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    return GameplayAbility != nullptr ? GameplayAbility->GetTargetType() : ERTSTargetType::NONE;
}

bool URTSAbilitySystemHelper::ShouldShowAbilityAsOrderInUI(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return false;
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    return GameplayAbility != nullptr ? GameplayAbility->ShouldShowAsOrderInUI() : false;
}

void URTSAbilitySystemHelper::CancelAbility(UAbilitySystemComponent* AbilitySystem,
                                            TSubclassOf<UGameplayAbility> Ability)
{
    if (AbilitySystem == nullptr || Ability == nullptr)
    {
        return;
    }

    AbilitySystem->CancelAbility(Ability->GetDefaultObject<UGameplayAbility>());
}

bool URTSAbilitySystemHelper::IsAbilityActive(UAbilitySystemComponent* AbilitySystem,
                                              TSubclassOf<UGameplayAbility> Ability)
{
    if (AbilitySystem == nullptr || Ability == nullptr)
    {
        return false;
    }

    FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromClass(Ability);

    return Spec->IsActive();
}

bool URTSAbilitySystemHelper::IsPassiveAbilityUnlocked(const UAbilitySystemComponent* AbilitySystem,
                                                       TSubclassOf<UGameplayAbility> Ability)
{
    if (!IsValid(AbilitySystem) || Ability == nullptr)
    {
        return false;
    }

    URTSGameplayAbility* GameplayAbility = Ability->GetDefaultObject<URTSGameplayAbility>();

    if (!IsValid(GameplayAbility))
    {
        return false;
    }

    return AbilitySystem->HasAllMatchingGameplayTags(GameplayAbility->GetActivationRequiredTags());
}

float URTSAbilitySystemHelper::GetAbilityRange(UAbilitySystemComponent* AbilitySystem,
                                               TSubclassOf<UGameplayAbility> Ability)
{
    URTSAbilitySystemComponent* RTSAbilitySystem = Cast<URTSAbilitySystemComponent>(AbilitySystem);
    if (RTSAbilitySystem == nullptr || !Ability->IsChildOf<URTSGameplayAbility>())
    {
        return 0.0f;
    }

    return RTSAbilitySystem->GetAbilityRange(*Ability);
}

ERTSAbilityProcessPolicy URTSAbilitySystemHelper::GetAbilityProcessPolicy(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return ERTSAbilityProcessPolicy::INSTANT;
    }

    URTSGameplayAbility* AbilityCDO = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    if (AbilityCDO == nullptr)
    {
        return ERTSAbilityProcessPolicy::INSTANT;
    }

    return AbilityCDO->GetAbilityProcessPolicy();
}

void URTSAbilitySystemHelper::GetAbilitySourceTagRequirements(TSubclassOf<UGameplayAbility> Ability,
                                                              FGameplayTagContainer& OutRequiredTags,
                                                              FGameplayTagContainer& OutBlockedTags)
{
    if (Ability == nullptr)
    {
        return;
    }

    URTSGameplayAbility* AbilityCDO = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    if (AbilityCDO == nullptr)
    {
        return;
    }

    return AbilityCDO->GetSourceTagRequirements(OutRequiredTags, OutBlockedTags);
}

void URTSAbilitySystemHelper::GetAbilityTargetTagRequirements(TSubclassOf<UGameplayAbility> Ability,
                                                              FGameplayTagContainer& OutRequiredTags,
                                                              FGameplayTagContainer& OutBlockedTags)
{
    if (Ability == nullptr)
    {
        return;
    }

    URTSGameplayAbility* AbilityCDO = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    if (AbilityCDO == nullptr)
    {
        return;
    }

    return AbilityCDO->GetTargetTagRequirements(OutRequiredTags, OutBlockedTags);
}

bool URTSAbilitySystemHelper::IsHumanPlayerAutoAbility(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return false;
    }

    URTSGameplayAbility* AbilityCDO = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    if (AbilityCDO == nullptr)
    {
        return false;
    }

    return AbilityCDO->IsHumanPlayerAutoAbility();
}

// ---------------------------------------------------------------------------------------------------
// Effects
// ---------------------------------------------------------------------------------------------------

float URTSAbilitySystemHelper::GetActiveEffectTimeRemaining(const UAbilitySystemComponent* AbilitySystem,
                                                            TSubclassOf<UGameplayEffect> Effect)
{
    float Time;
    float Duration;
    GetActiveEffectTimeRemainingAndDuration(AbilitySystem, Effect, Time, Duration);
    return Time;
}

float URTSAbilitySystemHelper::GetActiveEffectDuration(const UAbilitySystemComponent* AbilitySystem,
                                                       TSubclassOf<UGameplayEffect> Effect)
{
    float Time;
    float Duration;
    GetActiveEffectTimeRemainingAndDuration(AbilitySystem, Effect, Time, Duration);
    return Duration;
}

void URTSAbilitySystemHelper::GetActiveEffectTimeRemainingAndDuration(const UAbilitySystemComponent* AbilitySystem,
                                                                      TSubclassOf<UGameplayEffect> Effect,
                                                                      float& OutRemainingTime, float& OutDuration)
{
    // Make sure to always assign out references
    OutRemainingTime = 0.0f;
    OutDuration = 0.0f;

    if (Effect != nullptr && AbilitySystem != nullptr)
    {
        FGameplayEffectQuery Query;
        Query.EffectDefinition = Effect;
        TArray<TPair<float, float>> RemainingTimes = AbilitySystem->GetActiveEffectsTimeRemainingAndDuration(Query);

        for (int32 i = 0; i < RemainingTimes.Num(); ++i)
        {
            TPair<float, float> Pair = RemainingTimes[i];
            float Time = Pair.Key;
            float Duration = Pair.Value;

            if (Time > OutRemainingTime)
            {
                OutRemainingTime = Time;
            }

            if (Duration > OutDuration)
            {
                OutDuration = Duration;
            }
        }
    }
}

TArray<TSubclassOf<UGameplayEffect>>
URTSAbilitySystemHelper::GetActiveGameplayEffects(const UAbilitySystemComponent* AbilitySystem)
{
    TArray<TSubclassOf<UGameplayEffect>> OutEffects;
    if (AbilitySystem == nullptr)
    {
        return OutEffects;
    }

    TArray<FGameplayEffectSpec> OutEffectSpecs;
    AbilitySystem->GetAllActiveGameplayEffectSpecs(OutEffectSpecs);

    for (const FGameplayEffectSpec& EffectSpec : OutEffectSpecs)
    {
        OutEffects.Add(EffectSpec.Def->GetClass());
    }

    return OutEffects;
}

TArray<FGameplayEffectSpec>
URTSAbilitySystemHelper::GetActiveGameplayEffectSpecs(const UAbilitySystemComponent* AbilitySystem)
{
    TArray<FGameplayEffectSpec> OutEffectSpecs;
    AbilitySystem->GetAllActiveGameplayEffectSpecs(OutEffectSpecs);
    return OutEffectSpecs;
}

TSubclassOf<UGameplayEffect>
URTSAbilitySystemHelper::GetGameplayEffectSpecClass(const FGameplayEffectSpec& GameplayEffectSpec)
{
    return GameplayEffectSpec.Def->GetClass();
}

int32 URTSAbilitySystemHelper::GetGameplayEffectSpecStackCount(const FGameplayEffectSpec& GameplayEffectSpec)
{
    return GameplayEffectSpec.StackCount;
}

UTexture2D* URTSAbilitySystemHelper::GetEffectIcon(TSubclassOf<UGameplayEffect> Effect)
{
    if (Effect == nullptr)
    {
        return nullptr;
    }

    URTSGameplayEffect* GameplayEffect = Cast<URTSGameplayEffect>(Effect->GetDefaultObject<UGameplayEffect>());
    return GameplayEffect != nullptr ? GameplayEffect->GetIcon() : nullptr;
}

FText URTSAbilitySystemHelper::GetEffectName(TSubclassOf<UGameplayEffect> Effect)
{
    if (Effect == nullptr)
    {
        return FText();
    }

    URTSGameplayEffect* GameplayEffect = Cast<URTSGameplayEffect>(Effect->GetDefaultObject<UGameplayEffect>());
    return GameplayEffect != nullptr ? GameplayEffect->GetEffectName() : FText();
}

FText URTSAbilitySystemHelper::GetEffectDescription(TSubclassOf<UGameplayEffect> Effect)
{
    if (Effect == nullptr)
    {
        return FText();
    }

    URTSGameplayEffect* GameplayEffect = Cast<URTSGameplayEffect>(Effect->GetDefaultObject<UGameplayEffect>());
    return GameplayEffect != nullptr ? GameplayEffect->GetEffectDescription() : FText();
}

// ---------------------------------------------------------------------------------------------------
// Tags
// ---------------------------------------------------------------------------------------------------

FName URTSAbilitySystemHelper::GetLastTagName(FGameplayTag Tag)
{
    if (!Tag.IsValid())
    {
        return FName();
    }

    TArray<FName> TagNames;
    UGameplayTagsManager::Get().SplitGameplayTagFName(Tag, TagNames);

    return TagNames.Last();
}

FGameplayTagContainer URTSAbilitySystemHelper::GetGameplayTags(const UAbilitySystemComponent* AbilitySystem)
{
    FGameplayTagContainer TagContainer;
    if (AbilitySystem == nullptr)
    {
        return TagContainer;
    }

    AbilitySystem->GetOwnedGameplayTags(TagContainer);
    return TagContainer;
}

int32 URTSAbilitySystemHelper::GetGameplayTagCount(const UAbilitySystemComponent* AbilitySystem,
                                                   FGameplayTag TagToCheck)
{
    if (AbilitySystem == nullptr)
    {
        return 0;
    }

    return AbilitySystem->GetTagCount(TagToCheck);
}

bool URTSAbilitySystemHelper::HasMatchingGameplayTag(const UAbilitySystemComponent* AbilitySystem,
                                                     FGameplayTag TagToCheck)
{
    if (AbilitySystem == nullptr)
    {
        return false;
    }

    return AbilitySystem->HasMatchingGameplayTag(TagToCheck);
}

bool URTSAbilitySystemHelper::HasAllMatchingGameplayTags(const UAbilitySystemComponent* AbilitySystem,
                                                         const FGameplayTagContainer& TagContainer)
{
    if (AbilitySystem == nullptr)
    {
        return false;
    }

    return AbilitySystem->HasAllMatchingGameplayTags(TagContainer);
}

bool URTSAbilitySystemHelper::HasAnyMatchingGameplayTags(const UAbilitySystemComponent* AbilitySystem,
                                                         const FGameplayTagContainer& TagContainer)
{
    if (AbilitySystem == nullptr)
    {
        return false;
    }

    return AbilitySystem->HasAnyMatchingGameplayTags(TagContainer);
}

bool URTSAbilitySystemHelper::HasActorMatchingGameplayTag(const AActor* Actor, FGameplayTag TagToCheck)
{
    if (!IsValid(Actor))
    {
        return false;
    }

    return HasMatchingGameplayTag(Actor->FindComponentByClass<UAbilitySystemComponent>(), TagToCheck);
}

bool URTSAbilitySystemHelper::HasActorClassMatchingGameplayTag(TSubclassOf<AActor> ActorClass, FGameplayTag TagToCheck)
{
    URTSAbilitySystemComponent* AbilitySystemComponent =
        FindDefaultComponentByClass<URTSAbilitySystemComponent>(ActorClass);

    return IsValid(AbilitySystemComponent) && (AbilitySystemComponent->GetNameTag().MatchesTag(TagToCheck) ||
                                               AbilitySystemComponent->GetInitialTags().HasTag(TagToCheck));
}

bool URTSAbilitySystemHelper::HasActorAllMatchingGameplayTags(const AActor* Actor,
                                                              const FGameplayTagContainer& TagContainer)
{
    if (!IsValid(Actor))
    {
        return false;
    }

    return HasAllMatchingGameplayTags(Actor->FindComponentByClass<UAbilitySystemComponent>(), TagContainer);
}

bool URTSAbilitySystemHelper::HasActorAnyMatchingGameplayTags(const AActor* Actor,
                                                              const FGameplayTagContainer& TagContainer)
{
    if (!IsValid(Actor))
    {
        return false;
    }

    return HasAnyMatchingGameplayTags(Actor->FindComponentByClass<UAbilitySystemComponent>(), TagContainer);
}

void URTSAbilitySystemHelper::GetTags(const AActor* Actor, FGameplayTagContainer& OutGameplayTags)
{
    OutGameplayTags = OutGameplayTags.EmptyContainer;
    if (!IsValid(Actor))
    {
        return;
    }

    const UAbilitySystemComponent* AbilitySystem = Actor->FindComponentByClass<UAbilitySystemComponent>();
    if (AbilitySystem == nullptr)
    {
        return;
    }

    AbilitySystem->GetOwnedGameplayTags(OutGameplayTags);
}

void URTSAbilitySystemHelper::GetPlayerOwnerTags(const AActor* Actor, FGameplayTagContainer& OutGameplayTags)
{
    OutGameplayTags = OutGameplayTags.EmptyContainer;

    // NOTE(np): In A Year Of Rain, players themselves store player-specific tags in an ability system component.
    /*if (!IsValid(Actor))
    {
        return;
    }

    URTSOwnerComponent* OwnerComponent = Actor->FindComponentByClass<URTSOwnerComponent>();
    if (OwnerComponent == nullptr)
    {
        return;
    }

    ARTSPlayerState* PlayerState = OwnerComponent->GetPlayerOwner();
    if (PlayerState == nullptr)
    {
        return;
    }

    const UAbilitySystemComponent* AbilitySystem = PlayerState->FindComponentByClass<UAbilitySystemComponent>();

    AbilitySystem->GetOwnedGameplayTags(OutGameplayTags);*/
}

FGameplayTagContainer URTSAbilitySystemHelper::GetRelationshipTags(const AActor* Actor, const AActor* Other)
{

    FGameplayTagContainer RelationshipTags;
    if (Actor == nullptr || Other == nullptr)
    {
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Neutral());
    }

    else if (Actor == Other)
    {
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Friendly());
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Self());
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Visible());
    }

    else
    {
        // NOTE(np): In A Year Of Rain, we're adding more relationship tags based on the current owners of both units.
        /* const URTSOwnerComponent* ActorOwnerComponent = Actor->FindComponentByClass<URTSOwnerComponent>();
         const URTSOwnerComponent* OtherOwnerComponent = Other->FindComponentByClass<URTSOwnerComponent>();

         if (ActorOwnerComponent == nullptr || OtherOwnerComponent == nullptr)
         {
             RelationshipTags.AddTag(URTSGlobalTags::Relationship_Neutral());
         }

         else
         {
             const ARTSPlayerState* ActorPlayerState = ActorOwnerComponent->GetPlayerOwner();
             const ARTSPlayerState* OtherPlayerState = OtherOwnerComponent->GetPlayerOwner();

             GetRelationshipTagsFromPlayers(ActorPlayerState, OtherPlayerState, RelationshipTags);
         }*/

        if (!RelationshipTags.HasTag(URTSGlobalTags::Relationship_Visible()) && IsVisibleForActor(Actor, Other))
        {
            RelationshipTags.AddTag(URTSGlobalTags::Relationship_Visible());
        }
    }

    return RelationshipTags;
}

// NOTE(np): In A Year Of Rain, we're adding relationship tags based on the team assignments of both players.
//void URTSAbilitySystemHelper::GetRelationshipTagsFromPlayers(const ARTSPlayerState* ActorPlayerState,
//                                                             const ARTSPlayerState* OtherPlayerState,
//                                                             FGameplayTagContainer& OutRelationshipTags)
//{
//    
//    if (ActorPlayerState == nullptr || OtherPlayerState == nullptr)
//    {
//        OutRelationshipTags.AddTag(URTSGlobalTags::Relationship_Neutral());
//    }
//
//    else if (ActorPlayerState->HasArmisticeWithTeamOf(OtherPlayerState))
//    {
//        OutRelationshipTags.AddTag(URTSGlobalTags::Relationship_Friendly());
//    }
//
//    else if (!ActorPlayerState->IsSameTeamAs(OtherPlayerState))
//    {
//        OutRelationshipTags.AddTag(URTSGlobalTags::Relationship_Hostile());
//    }
//
//    else
//    {
//        OutRelationshipTags.AddTag(URTSGlobalTags::Relationship_Friendly());
//        OutRelationshipTags.AddTag(URTSGlobalTags::Relationship_Visible());
//        if (ActorPlayerState->PlayerIndex == OtherPlayerState->PlayerIndex)
//        {
//            OutRelationshipTags.AddTag(URTSGlobalTags::Relationship_SamePlayer());
//        }
//    }
//}

void URTSAbilitySystemHelper::GetSourceAndTargetTags(const AActor* SourceActor, const AActor* TargetActor,
                                                     FGameplayTagContainer& OutSourceTags,
                                                     FGameplayTagContainer& OutTargetTags)
{
    GetTags(SourceActor, OutSourceTags);
    GetTags(TargetActor, OutTargetTags);

    FGameplayTagContainer RelationshipTags = URTSAbilitySystemHelper::GetRelationshipTags(SourceActor, TargetActor);

    OutSourceTags.AppendTags(RelationshipTags);
    OutTargetTags.AppendTags(RelationshipTags);
}

FGameplayTagContainer
URTSAbilitySystemHelper::UnionGameplayTagContainers(const FGameplayTagContainer& FirstTagContainer,
                                                    const FGameplayTagContainer& SecondTagContainer)
{
    FGameplayTagContainer Result;
    Result.AppendTags(FirstTagContainer);
    Result.AppendTags(SecondTagContainer);

    return Result;
}

void URTSAbilitySystemHelper::ChangeTag(AActor* Actor, const FGameplayTag& Tag, bool bExistence)
{
    if (!IsValid(Actor))
    {
        return;
    }

    URTSAbilitySystemComponent* AbilitySystem =
        Cast<URTSAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
    if (AbilitySystem == nullptr)
    {
        return;
    }

    bool bHasTag = AbilitySystem->HasMatchingGameplayTag(Tag);
    if (bExistence && !bHasTag)
    {
        AbilitySystem->AddTag(Tag);
    }

    if (!bExistence && bHasTag)
    {
        AbilitySystem->RemoveTag(Tag);
    }
}

bool URTSAbilitySystemHelper::DoesSatisfyTagRequirements(const FGameplayTagContainer& Tags,
                                                         const FGameplayTagContainer& RequiredTags,
                                                         const FGameplayTagContainer& BlockedTags)
{
    if (BlockedTags.Num() || RequiredTags.Num())
    {
        if (Tags.HasAny(BlockedTags))
        {
            return false;
        }

        if (!Tags.HasAll(RequiredTags))
        {
            return false;
        }
    }

    return true;
}

bool URTSAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(const FGameplayTagContainer& Tags,
                                                                   const FGameplayTagContainer& InRequiredTags,
                                                                   const FGameplayTagContainer& InBlockedTags,
                                                                   FGameplayTagContainer& OutMissingTags,
                                                                   FGameplayTagContainer& OutBlockingTags)
{
    bool bSuccess = true;
    for (FGameplayTag Tag : InBlockedTags)
    {
        if (Tags.HasTag(Tag))
        {
            bSuccess = false;
            OutBlockingTags.AddTag(Tag);
        }
    }

    for (FGameplayTag Tag : InRequiredTags)
    {
        if (!Tags.HasTag(Tag))
        {
            bSuccess = false;
            OutMissingTags.AddTag(Tag);
        }
    }

    return bSuccess;
}

FGameplayTagContainer URTSAbilitySystemHelper::FilterForTagsWithParentTag(const FGameplayTagContainer& TagContainer,
                                                                          FGameplayTag ParentTag)
{
    FGameplayTagContainer TagsToReturn;
    for (FGameplayTag Tag : TagContainer)
    {
        if (Tag.MatchesTag(ParentTag))
        {
            TagsToReturn.AddTag(Tag);
        }
    }

    return TagsToReturn;
}

void URTSAbilitySystemHelper::CreateGameplayEventData(AActor* Source, const FRTSOrderTargetData& TargetData,
                                                      TSubclassOf<UGameplayAbility> Ability,
                                                      FGameplayEventData& OutEventData)
{
    if (Source == nullptr)
    {
        return;
    }

    URTSAbilitySystemComponent* AbilitySystem =
        Cast<URTSAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source));

    FGameplayTagContainer InstigatorTags;
    URTSAbilitySystemHelper::GetTags(Source, InstigatorTags);

    URTSGameplayAbility* RTSAbility =
        Ability ? Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>()) : nullptr;

    OutEventData.EventTag = RTSAbility ? RTSAbility->GetEventTriggerTag() : FGameplayTag();
    OutEventData.Instigator = Source;
    OutEventData.Target = TargetData.Actor;
    OutEventData.OptionalObject = nullptr;
    OutEventData.OptionalObject2 = nullptr;
    OutEventData.InstigatorTags = InstigatorTags;
    OutEventData.TargetTags = TargetData.TargetTags;
    OutEventData.EventMagnitude = AbilitySystem->GetLevel();
    OutEventData.TargetData =
        CreateAbilityTargetDataFromOrderTargetData(Source, TargetData, GetAbilityTargetType(Ability));
}

void URTSAbilitySystemHelper::CreateGameplayEventDataWithEventTag(AActor* Source, AActor* Target, FGameplayTag EventTag,
                                                                  FGameplayEventData& OutEventData)
{

    if (Source == nullptr || Target == nullptr)
    {
        return;
    }

    URTSAbilitySystemComponent* AbilitySystem =
        Cast<URTSAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source));

    FGameplayTagContainer SourceTags;
    URTSAbilitySystemHelper::GetTags(Source, SourceTags);

    FGameplayTagContainer TargetTags;
    URTSAbilitySystemHelper::GetTags(Target, TargetTags);

    OutEventData.EventTag = EventTag;
    OutEventData.Instigator = Source;
    OutEventData.Target = Target;
    OutEventData.OptionalObject = nullptr;
    OutEventData.OptionalObject2 = nullptr;
    OutEventData.InstigatorTags = SourceTags;
    OutEventData.TargetTags = TargetTags;
    OutEventData.EventMagnitude = AbilitySystem->GetLevel();
    OutEventData.TargetData = nullptr;
}

int32 URTSAbilitySystemHelper::SendGameplayEvent(AActor* Actor, FGameplayEventData Payload)
{
    if (IsValid(Actor))
    {
        IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
        if (AbilitySystemInterface != nullptr)
        {
            UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
            if (AbilitySystemComponent != nullptr)
            {
                FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
                return AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
            }
        }
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------
// Curve Tables
// ---------------------------------------------------------------------------------------------------

float URTSAbilitySystemHelper::GetValueAtLevel(const FScalableFloat& ScalableFloat, float Level)
{
    return ScalableFloat.GetValueAtLevel(Level);
}

TArray<FGameplayAttribute> URTSAbilitySystemHelper::FindGameplayAttributes()
{
    TArray<FGameplayAttribute> Attributes;
    for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
    {
        UClass* Class = *ClassIt;
        if (Class->IsChildOf(UAttributeSet::StaticClass()) && !Class->ClassGeneratedBy)
        {
            for (TFieldIterator<UProperty> PropertyIt(Class, EFieldIteratorFlags::ExcludeSuper); PropertyIt;
                 ++PropertyIt)
            {
                UProperty* Property = *PropertyIt;
                Attributes.Add(FGameplayAttribute(Property));
            }
        }
    }

    return Attributes;
}

FGameplayAbilityTargetDataHandle URTSAbilitySystemHelper::CreateAbilityTargetDataFromOrderTargetData(
    AActor* OrderedActor, const FRTSOrderTargetData& OrderTargetData, ERTSTargetType TargetType)
{
    switch (TargetType)
    {
        case ERTSTargetType::ACTOR:
        {
            FGameplayAbilityTargetData_ActorArray* ActorData = new FGameplayAbilityTargetData_ActorArray();
            ActorData->TargetActorArray.Add(OrderTargetData.Actor);
            return FGameplayAbilityTargetDataHandle(ActorData);
        }
        case ERTSTargetType::LOCATION:
        case ERTSTargetType::DIRECTION:
        {
            FTransform Transform;

            FGameplayAbilityTargetingLocationInfo SourceLocation;
            SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
            Transform.SetLocation(OrderedActor->GetActorLocation());
            SourceLocation.LiteralTransform = Transform;

            FGameplayAbilityTargetingLocationInfo TargetLocation;
            TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
            Transform.SetLocation(FVector(OrderTargetData.Location.X, OrderTargetData.Location.Y, 0.0f));
            TargetLocation.LiteralTransform = Transform;

            FGameplayAbilityTargetData_LocationInfo* LocationData = new FGameplayAbilityTargetData_LocationInfo();
            LocationData->SourceLocation = SourceLocation;
            LocationData->TargetLocation = TargetLocation;

            return FGameplayAbilityTargetDataHandle(LocationData);
        }
        default:
            return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------
// Helper
// ---------------------------------------------------------------------------------------------------

UActorComponent* URTSAbilitySystemHelper::FindDefaultComponentByClass(const TSubclassOf<AActor> InActorClass,
    const TSubclassOf<UActorComponent> InComponentClass)
{
    if (!IsValid(InActorClass))
    {
        return nullptr;
    }

    // Check CDO.
    AActor* ActorCDO = InActorClass->GetDefaultObject<AActor>();
    UActorComponent* FoundComponent = ActorCDO->FindComponentByClass(InComponentClass);

    if (FoundComponent != nullptr)
    {
        return FoundComponent;
    }

    // Check blueprint nodes. Components added in blueprint editor only (and not in code) are not available from
    // CDO.
    UBlueprintGeneratedClass* RootBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(InActorClass);
    UClass* ActorClass = InActorClass;

    // Go down the inheritance tree to find nodes that were added to parent blueprints of our blueprint graph.
    do
    {
        UBlueprintGeneratedClass* ActorBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(ActorClass);
        if (!ActorBlueprintGeneratedClass)
        {
            return nullptr;
        }

        const TArray<USCS_Node*>& ActorBlueprintNodes =
            ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes();

        for (USCS_Node* Node : ActorBlueprintNodes)
        {
            if (Node->ComponentClass->IsChildOf(InComponentClass))
            {
                return Node->GetActualComponentTemplate(RootBlueprintGeneratedClass);
            }
        }

        ActorClass = Cast<UClass>(ActorClass->GetSuperStruct());

    } while (ActorClass != AActor::StaticClass());

    return nullptr;
}

bool URTSAbilitySystemHelper::IsVisibleForActor(const AActor* Actor, const AActor* Other)
{
    if (Actor == nullptr || Other == nullptr)
    {
        return false;
    }

    // NOTE(np): In A Year Of Rain, the visible component stores for which players a units is currently visible.
    /*const URTSVisibleComponent* OtherVisibleComponent = Other->FindComponentByClass<URTSVisibleComponent>();
    if (OtherVisibleComponent != nullptr)
    {
        if (Other->HasAuthority())
        {
            return OtherVisibleComponent->IsVisibleForPlayer(Cast<AController>(Actor->GetOwner()));
        }

        return OtherVisibleComponent->IsVisibleForLocalClient();
    }

    else*/
    {
        // No visible component? It must be always visible!
        return true;
    }
}

TArray<TSubclassOf<UGameplayAbility>>
URTSAbilitySystemHelper::GetGrantedAbilitiesFromGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    TArray<TSubclassOf<UGameplayAbility>> GrantedAbilites;

    if (!IsValid(GameplayEffectClass))
    {
        return GrantedAbilites;
    }

    UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();

    for (FGameplayAbilitySpecDef& AbilityDef : GameplayEffect->GrantedAbilities)
    {
        GrantedAbilites.Add(AbilityDef.Ability);
    }

    return GrantedAbilites;
}

bool URTSAbilitySystemHelper::GetItemAbilityIndex(UAbilitySystemComponent* AbilitySystem,
                                                  TSubclassOf<UGameplayAbility> AbilityClass, int32& OutIndex)
{
    if (AbilityClass == nullptr)
    {
        return false;
    }

    if (!IsValid(AbilitySystem))
    {
        return false;
    }

    URTSAbilitySystemComponent* RTSAbilitySystemComponent = Cast<URTSAbilitySystemComponent>(AbilitySystem);

    if (!IsValid(RTSAbilitySystemComponent))
    {
        return false;
    }

    const TArray<TSubclassOf<UGameplayAbility>>& ItemAbilities = RTSAbilitySystemComponent->GetItemAbilities();

    for (int32 i = 0; i < ItemAbilities.Num(); i++)
    {
        if (ItemAbilities[i] == AbilityClass)
        {
            OutIndex = i;
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------------------------------------
// GameplayCue
// ---------------------------------------------------------------------------------------------------

void URTSAbilitySystemHelper::ExecuteGameplayCueWithParamsUnattached(
    AActor* Actor, FGameplayTag AreaOfEffectGameplayCue, const FGameplayCueParameters& GameplayCueParameters)
{

    // Activate GameplayCue.
    if (!IsValid(Actor))
    {
        return;
    }

    if (!AreaOfEffectGameplayCue.IsValid())
    {
        return;
    }

    UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
        Actor, AreaOfEffectGameplayCue, EGameplayCueEvent::Executed, GameplayCueParameters);
}

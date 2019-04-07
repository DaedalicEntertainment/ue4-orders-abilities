#include "AbilitySystem/RTSAbilitySystemComponent.h"

#include "OrdersAbilities.h"

#include "UnrealNetwork.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"

#include "AbilitySystem/RTSAbilitySystemHelper.h"
#include "AbilitySystem/RTSAttributeSet.h"
#include "AbilitySystem/RTSGameplayAbility.h"
#include "AbilitySystem/RTSGlobalTags.h"
#include "AbilitySystem/RTSInitialStatusTagsProvider.h"

URTSAbilitySystemComponent::URTSAbilitySystemComponent()
{
    Level = 1;
    CollectedXP = 0;
    AbilityPoints = 0;
}

void URTSAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URTSAbilitySystemComponent, CollectedXP);
    DOREPLIFETIME(URTSAbilitySystemComponent, AbilityPoints);
    DOREPLIFETIME(URTSAbilitySystemComponent, ItemAbilities);

    // Our pawns are owned by PlayerControllers, but possessed by AIControllers.
    // As APawn::GetNetConnection() is forwarded to their respective controller,
    // we need to change the replication condition for ActivatableAbilities in order
    // to properly replicate to clients.
    DOREPLIFETIME_CHANGE_CONDITION(URTSAbilitySystemComponent, ActivatableAbilities, COND_None);
}

FName URTSAbilitySystemComponent::GetName() const
{
    static FName Name = URTSAbilitySystemHelper::GetLastTagName(NameTag);
    return Name;
}

FGameplayTag URTSAbilitySystemComponent::GetNameTag() const
{
    return NameTag;
}

void URTSAbilitySystemComponent::SetLevel(int32 NewLevel)
{
    AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    if (NewLevel >= MaxLevel)
    {
        CollectedXP = GetTotalXPRequiredForLevel(MaxLevel - 1);
        NewLevel = MaxLevel;
    }

    if (Level == NewLevel)
    {
        return;
    }

    if (Owner->HasAuthority())
    {
        if (NewLevel > 1)
        {
            FGameplayEffectSpecHandle Effect = MakeOutgoingSpec(LevelUpEffect, NewLevel, MakeEffectContext());
            if (Effect.IsValid())
            {
                ApplyGameplayEffectSpecToSelf(*Effect.Data.Get());
            }
        }

        InitializeAttributes(NewLevel, false);

        // Grant ability points.
        SetAbilityPoints(AbilityPoints + NewLevel - Level);
    }

    Level = NewLevel;
}

int32 URTSAbilitySystemComponent::GetLevel() const
{
    return Level;
}

float URTSAbilitySystemComponent::GetGrantedXP() const
{
    return GrantedXP.GetValueAtLevel(Level);
}

float URTSAbilitySystemComponent::GetTotalXPRequiredForLevel(int32 InLevel) const
{
    float ValueAtLevel = 0;

    for (int32 CurrentLevel = 1; CurrentLevel <= InLevel; ++CurrentLevel)
    {
        ValueAtLevel += XPPerLevel.GetValueAtLevel(CurrentLevel);
    }

    return ValueAtLevel;
}

float URTSAbilitySystemComponent::GetCurrentLevelXP() const
{
    float CurrentLevelXP = Level > 1 ? GetTotalXPRequiredForLevel(Level - 1) : 0;
    return CollectedXP - CurrentLevelXP;
}

float URTSAbilitySystemComponent::GetNextLevelXP() const
{
    return XPPerLevel.GetValueAtLevel(Level);
}

float URTSAbilitySystemComponent::GetCurrentLevelXPProgress() const
{
    float CurrentLevelXP = GetCurrentLevelXP();
    float NextLevelXP = GetNextLevelXP();

    if (NextLevelXP <= 0.0f)
    {
        return 1.0f;
    }

    return CurrentLevelXP / NextLevelXP;
}

float URTSAbilitySystemComponent::GetCollectedXP() const
{
    return CollectedXP;
}

void URTSAbilitySystemComponent::AddCollectedXP(float AdditionalCollectedXP)
{
    if (!bCanLevelUp)
    {
        return;
    }

    if (Level == MaxLevel)
    {
        return;
    }

    float OldCollectedXP = CollectedXP;
    CollectedXP += AdditionalCollectedXP;

    // Notify listers.
    NotifyOnCollectedXPChanged(OldCollectedXP, CollectedXP);

    UpdateLevel();
}

bool URTSAbilitySystemComponent::CanLevelUp() const
{
    return bCanLevelUp;
}

int32 URTSAbilitySystemComponent::GetAbilityPoints() const
{
    return AbilityPoints;
}

void URTSAbilitySystemComponent::SetAbilityPoints(int32 NewAbilityPoints)
{
    if (AbilityPoints == NewAbilityPoints)
    {
        return;
    }

    int32 OldAbilityPoints = AbilityPoints;
    AbilityPoints = NewAbilityPoints;

    NotifyOnAbilityPointsChanged(OldAbilityPoints, NewAbilityPoints);
}

void URTSAbilitySystemComponent::IncreaseAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass,
                                                      bool bUseAbilityPoint /*= true*/)
{
    if (AbilityClass == nullptr)
    {
        UE_LOG(LogRTS, Error, TEXT("%s tried to increase the level of an ability, but no ability was specified."),
               *GetOwner()->GetName());
        return;
    }

    // Check available ability points.
    if (bUseAbilityPoint && AbilityPoints <= 0)
    {
        UE_LOG(LogRTS, Error,
               TEXT("%s tried to increase the level of ability %s, but no ability points were available."),
               *GetOwner()->GetName(), *AbilityClass->GetName());
        return;
    }

    // Find ability.
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!IsValid(AbilitySpec.Ability) || AbilitySpec.Ability->GetClass() != AbilityClass)
        {
            continue;
        }

        // Check max level.
        if (AbilitySpec.Level == URTSAbilitySystemHelper::GetAbilityMaxLevel(this, AbilityClass))
        {
            UE_LOG(LogRTS, Error,
                   TEXT("%s tried to increase the level of ability %s above %i, but that ability is already at maximum "
                        "level."),
                   *GetOwner()->GetName(), *AbilityClass->GetName(), AbilitySpec.Level);
            return;
        }

        // Increase level.
        ++AbilitySpec.Level;

        // Notify instances.
        for (UGameplayAbility* Ability : AbilitySpec.GetAbilityInstances())
        {
            URTSGameplayAbility* RTSAbility = Cast<URTSGameplayAbility>(Ability);
            if (Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
            {
                RTSAbility->OnAbilityLevelChanged(AbilitySpec.Level);
            }
        }

        if (bUseAbilityPoint)
        {
            // Remove ability point.
            SetAbilityPoints(AbilityPoints - 1);
        }

        UE_LOG(LogRTS, Log, TEXT("%s increased the level of ability %s to %i. Remaining ability points: %i."),
               *GetOwner()->GetName(), *AbilityClass->GetName(), AbilitySpec.Level, AbilityPoints);

        return;
    }

    if (UnlockableAbilities.Contains(AbilityClass))
    {
        // Grant ability.
        GiveAbility(FGameplayAbilitySpec((AbilityClass->GetDefaultObject<UGameplayAbility>()), 1));

        if (bUseAbilityPoint)
        {
            // Remove ability point.
            SetAbilityPoints(AbilityPoints - 1);
        }

        UE_LOG(LogRTS, Log, TEXT("%s learned new ability %s. Remaining ability points: %i."), *GetOwner()->GetName(),
               *AbilityClass->GetName(), AbilityPoints);

        return;
    }

    UE_LOG(LogRTS, Error, TEXT("%s tried to increase the level of ability %s, but that ability wasn't found."),
           *GetOwner()->GetName(), *AbilityClass->GetName());
}

FGameplayTagContainer URTSAbilitySystemComponent::GetInitialTags() const
{
    return InitialTags;
}

const TArray<TSubclassOf<UGameplayAbility>>& URTSAbilitySystemComponent::GetAbilities() const
{
    return Abilities;
}

const TArray<TSubclassOf<UGameplayAbility>>& URTSAbilitySystemComponent::GetItemAbilities() const
{
    return ItemAbilities;
}

const TArray<TSubclassOf<UGameplayAbility>>& URTSAbilitySystemComponent::GetUnlockableAbilities() const
{
    return UnlockableAbilities;
}

TArray<TSubclassOf<UGameplayAbility>> URTSAbilitySystemComponent::GetInitialAndUnlockableAbilities() const
{
    TArray<TSubclassOf<UGameplayAbility>> OutAbilities;

    for (TSubclassOf<UGameplayAbility> InitialAbility : Abilities)
    {
        OutAbilities.Add(InitialAbility);
    }

    for (TSubclassOf<UGameplayAbility> UnlockableAbility : UnlockableAbilities)
    {
        OutAbilities.Add(UnlockableAbility);
    }

    return OutAbilities;
}

void URTSAbilitySystemComponent::AddItemAbility(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    if (!IsValid(GameplayEffectClass))
    {
        return;
    }

    UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
    ApplyGameplayEffectToSelf(GameplayEffect, 0.0f, FGameplayEffectContextHandle());

    // Hint: Only one granted ability is supported at this point
    // The current use of the OrderData-index would mess things up, if one
    // Item GameplayEffect would grant more than one ability
    if (GameplayEffect->GrantedAbilities.Num() > 1)
    {
        return;
    }

    if (GameplayEffect->GrantedAbilities.Num() == 0)
    {
        return;
    }

    for (FGameplayAbilitySpecDef& ItemAbilityDef : GameplayEffect->GrantedAbilities)
    {
        ItemAbilities.Add(ItemAbilityDef.Ability);
    }
}

void URTSAbilitySystemComponent::RemoveItemAbility(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
    if (!IsValid(GameplayEffectClass))
    {
        return;
    }

    UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
    FGameplayTagContainer& TagContainer = GameplayEffect->InheritableGameplayEffectTags.CombinedTags;

    RemoveActiveEffectsWithTags(TagContainer);
}

void URTSAbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();

    if (Owner == nullptr)
    {
        return;
    }

    // Register ability ended callback.
    OnAbilityEnded.AddUObject(this, &URTSAbilitySystemComponent::AbilityEndedCallback);

    if (!Owner->HasAuthority())
    {
        // Don't initialize the ability system component on client. This should only be done on the server.
        return;
    }

    // Initialize the ability system component.
    InitAbilityActorInfo(Owner, Owner);

    // Initialize the attributes.
    InitializeAttributes(Level, true);

    // Set the current XP based on the initial level.
    if (Level != MaxLevel)
    {
        CollectedXP = GetTotalXPRequiredForLevel(Level - 1);
    }

    else
    {
        CollectedXP = GetTotalXPRequiredForLevel(Level);
    }

    // Initialize the abilities.
    for (TSubclassOf<UGameplayAbility> Ability : Abilities)
    {
        if (Ability != nullptr)
        {
            GiveAbility(FGameplayAbilitySpec((Ability->GetDefaultObject<UGameplayAbility>()), Level));
        }
    }

    for (auto& InitialUnlockableAbilityLevel : InitialUnlockableAbilityLevels)
    {
        TSubclassOf<UGameplayAbility> Ability = InitialUnlockableAbilityLevel.Key;
        int32 AbilityLevel = InitialUnlockableAbilityLevel.Value;

        if (Ability != nullptr && AbilityLevel > 0)
        {
            GiveAbility(FGameplayAbilitySpec((Ability->GetDefaultObject<UGameplayAbility>()), AbilityLevel));
        }
    }

    // Initial Tags
    ApplyInitialTags();

    // NOTE(np): In A Year Of Rain, the health component provides an OnKilled event.
    //// Register an OnKilled listener
    //URTSHealthComponent* HealthComponent = Owner->FindComponentByClass<URTSHealthComponent>();
    //if (HealthComponent != nullptr)
    //{
    //    HealthComponent->OnKilled.AddDynamic(this, &URTSAbilitySystemComponent::OnKilled);
    //}

    // NOTE(np): In A Year Of Rain, the game mode provides a generic event for killed actors.
    //// Register a listener for units that has been killed to gain XP.
    //if (bCanLevelUp)
    //{
    //    ARTSGameMode* GameMode = URTSUtilities::GetRTSGameMode(this);
    //    GameMode->OnActorKilled.AddDynamic(this, &URTSAbilitySystemComponent::OnAnyActorKilled);
    //}

    // NOTE(np): In A Year Of Rain, units can change their owner at runtime (e.g. rescued units in story campaign).
    //// Make sure to adjust the tags when the owner of this component changes.
    //URTSOwnerComponent* OwnerComponent = Owner->FindComponentByClass<URTSOwnerComponent>();
    //if (OwnerComponent != nullptr)
    //{
    //    RegisterTransferPlayerTags(Cast<APlayerState>(OwnerComponent->GetPlayerOwner()));
    //    OwnerComponent->OnOwnerChanged.AddDynamic(this, &URTSAbilitySystemComponent::OnOwnerChanged);
    //}
}

void URTSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    Super::OnGiveAbility(AbilitySpec);

    if (!AbilitySpec.Ability)
    {
        return;
    }

    URTSGameplayAbility* RTSGameplayAbility = Cast<URTSGameplayAbility>(AbilitySpec.Ability);

    const TArray<FAbilityTriggerData>& AbilityTriggerData = RTSGameplayAbility->GetAbilityTriggerData();

    for (const FAbilityTriggerData& TriggerData : AbilityTriggerData)
    {
        FGameplayTag EventTag = TriggerData.TriggerTag;

        if (TriggerData.TriggerSource != EGameplayAbilityTriggerSource::GameplayEvent)
        {

            if (GetTagCount(TriggerData.TriggerTag))
            {
                UAbilitySystemComponent::TryActivateAbility(AbilitySpec.Handle, false);
                return;
            }
        }
    }
}

void URTSAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    Super::OnRemoveAbility(AbilitySpec);
}

void URTSAbilitySystemComponent::InitializeAttributes(int AttributeLevel, bool bInitialInit)
{
    if (!NameTag.IsValid())
    {
        return;
    }

    if (DefaultStartingData.Num() == 0)
    {
        return;
    }

    FName GroupName = URTSAbilitySystemHelper::GetLastTagName(NameTag);

    // Note that this might cause a crash when no valid paths to data tables where specified in the 'Game.ini' file.
    FAttributeSetInitter* AttributeInitter = UAbilitySystemGlobals::Get().GetAttributeSetInitter();

    // This is a work around for a bug that happens at least in the editor. It might be that the 'SpawnedAttributes'
    // contains nullptr entries for some for some unknown reason. This has properly something to do with serialization.
    // 'AttributeInitter->InitAttributeSetDefaults' will crash when the array contains a nullptr.
    for (int32 i = SpawnedAttributes.Num() - 1; i >= 0; --i)
    {
        if (SpawnedAttributes[i] == nullptr)
        {
            SpawnedAttributes.RemoveAt(i);
        }
    }

    UE_LOG(LogRTS, Verbose, TEXT("Initializing attributes of %s with group name %s..."), *GetOwner()->GetName(),
           *GroupName.ToString());

    AttributeInitter->InitAttributeSetDefaults(this, GroupName, AttributeLevel, bInitialInit);

    for (UAttributeSet* AttributeSet : SpawnedAttributes)
    {
        URTSAttributeSet* RTSAttributeSet = Cast<URTSAttributeSet>(AttributeSet);
        if (RTSAttributeSet != nullptr)
        {
            RTSAttributeSet->PostInitializeProperties(bInitialInit);
        }
    }
}

float URTSAbilitySystemComponent::GetAttributeValueFromCurveTable(const FGameplayAttribute& Attribute, int32 InLevel)
{
    if (!NameTag.IsValid())
    {
        return 0.0f;
    }

    // Note that this might cause a crash when no valid paths to data tables where specified in the 'Game.ini' file.
    FAttributeSetInitter* AttributeInitter = UAbilitySystemGlobals::Get().GetAttributeSetInitter();
    TArray<float> Values = AttributeInitter->GetAttributeSetValues(
        Attribute.GetAttributeSetClass(), Attribute.GetUProperty(), URTSAbilitySystemHelper::GetLastTagName(NameTag));

    if (!Values.IsValidIndex(InLevel - 1))
    {
        UE_LOG(LogRTS, Error, TEXT("Invalid level %i for attribute %s of %s."), InLevel, *Attribute.GetName(),
               *GetOwner()->GetName());
        return 0.0f;
    }

    return Values[InLevel - 1];
}

void URTSAbilitySystemComponent::OnKilled(AController* PreviousOwner, AActor* DamageCauser, AActor* KilledUnit)
{
    AddTags(TagsToAddOnDeath);
    RemoveTags(TagsToRemoveOnDeath);
    RemoveTags(InitialStatusTags);
}

void URTSAbilitySystemComponent::OnAnyActorKilled(AActor* KilledActor, AController* PreviousOwner, AActor* DamageCauser)
{
    if (KilledActor == nullptr)
    {
        return;
    }

    AActor* Owner = GetOwner();

    // Check if we've been killed.
    if (KilledActor == Owner)
    {
        return;
    }

    // NOTE(np): In A Year Of Rain, the owner component specified the current owner of a unit (for replication).
    //URTSOwnerComponent* OwnerComponent = Owner->FindComponentByClass<URTSOwnerComponent>();

    //if (!IsValid(OwnerComponent))
    //{
    //    return;
    //}

    //// Check if belonging to same team.
    //// Note that the OwnerComponent owner if KilledActor is no longer valid at this point, so we need to use
    //// PreviousOwner here.
    //if (IsValid(PreviousOwner) && IsValid(OwnerComponent->GetPlayerOwner()) &&
    //    OwnerComponent->GetPlayerOwner()->IsSameTeamAs(Cast<ARTSPlayerState>(PreviousOwner->PlayerState)))
    //{
    //    return;
    //}

    //// Check if killer belongs to our team.
    //if (!URTSUtilities::AreInSameTeam(GetOwner(), DamageCauser))
    //{
    //    return;
    //}

    URTSAbilitySystemComponent* KilledActorAbilitySystem =
        Cast<URTSAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(KilledActor));

    if (KilledActorAbilitySystem == nullptr)
    {
        return;
    }

    // Note that this calculation must be changed when their are more then one hero per player.
    float XPCollectedForKill = KilledActorAbilitySystem->GetGrantedXP();
    AddCollectedXP(XPCollectedForKill);
}

void URTSAbilitySystemComponent::ApplyInitialTags()
{
    InitialStatusTags = InitialStatusTags.EmptyContainer;
    FindAttributeSetStatusTags(InitialStatusTags);
    FindComponentStatusTags(InitialStatusTags);

    if (NameTag.IsValid())
    {
        AddTag(NameTag);
    }

    AddTags(InitialTags);
    AddTags(InitialStatusTags);
}

void URTSAbilitySystemComponent::OnOwnerChanged(APlayerState* PreviousOwner, APlayerState* NewOwner)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (IsValid(NewOwner))
    {
        if (IsValid(PreviousOwner))
        {
            UnregisterTransferPlayerTags(PreviousOwner);
        }
        RegisterTransferPlayerTags(NewOwner);
    }
}

void URTSAbilitySystemComponent::UnregisterTransferPlayerTags(APlayerState* PlayerOwner)
{
    if (IsValid(PlayerOwner))
    {
        // Remove the tags we got from our previous owner.
        FGameplayTagContainer PlayerTagsToRemove;
        for (FGameplayTag Tag : TransferPlayerTags)
        {
            if (HasMatchingGameplayTag(Tag))
            {
                PlayerTagsToRemove.AddTagFast(Tag);
            }
        }

        RemoveTags(PlayerTagsToRemove);

        // Register a callback for each
        UAbilitySystemComponent* PlayerOwnerAbilitySystem =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerOwner);

        if (PlayerOwnerAbilitySystem != nullptr)
        {
            for (TPair<FGameplayTag, FDelegateHandle> Pair : RegisteredOwnerTagEventHandles)
            {
                FOnGameplayEffectTagCountChanged& Delegate =
                    PlayerOwnerAbilitySystem->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved);

                Delegate.Remove(Pair.Value);
            }
        }
    }
}

void URTSAbilitySystemComponent::RegisterTransferPlayerTags(APlayerState* PlayerOwner)
{
    if (IsValid(PlayerOwner))
    {
        UAbilitySystemComponent* PlayerOwnerAbilitySystem =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerOwner);

        if (PlayerOwnerAbilitySystem != nullptr)
        {
            // Transfer all important tags from the player owner.
            FGameplayTagContainer TagsToAdd;
            for (FGameplayTag Tag : TransferPlayerTags)
            {
                if (PlayerOwnerAbilitySystem->HasMatchingGameplayTag(Tag))
                {
                    TagsToAdd.AddTag(Tag);
                }
            }

            AddTags(TagsToAdd);

            // Increase ability levels.
            for (auto& IncreaseAbilityLevelPlayerTag : IncreaseAbilityLevelPlayerTags)
            {
                if (PlayerOwnerAbilitySystem->HasMatchingGameplayTag(IncreaseAbilityLevelPlayerTag.Key))
                {
                    IncreaseAbilityLevel(IncreaseAbilityLevelPlayerTag.Value, false);
                }
            }

            // Register a callback for each of the tags to check if it was added to or removed from the owner.
            for (FGameplayTag Tag : TransferPlayerTags)
            {
                FOnGameplayEffectTagCountChanged& Delegate =
                    PlayerOwnerAbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);

                FDelegateHandle DelegateHandle =
                    Delegate.AddUObject(this, &URTSAbilitySystemComponent::OnOwningPlayerTagAddedOrRemoved);
                RegisteredOwnerTagEventHandles.Add(Tag, DelegateHandle);
            }
        }
    }
}

void URTSAbilitySystemComponent::AddTags(const FGameplayTagContainer& Tags)
{
    const AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    if (!Owner->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("Client tried to add the tags '%s' to '%s'. Tags should only be added on the server."),
               *Tags.ToString(), *Owner->GetName());

        return;
    }

    if (bLogTagChanges)
    {
        for (const FGameplayTag& Tag : Tags)
        {
            if (!HasMatchingGameplayTag(Tag))
            {
                UE_LOG(LogRTS, Log, TEXT("Adding tag %s to %s."), *Tag.ToString(), *Owner->GetName());
            }
        }
    }

    AddLooseGameplayTags(Tags);
    AddMinimalReplicationGameplayTags(Tags);
}

void URTSAbilitySystemComponent::RemoveTags(const FGameplayTagContainer& Tags)
{
    const AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    if (!Owner->HasAuthority())
    {
        UE_LOG(LogRTS, Error,
               TEXT("Client tried to remove the tags '%s' from '%s'. Tags should only be removed on the server."),
               *Tags.ToString(), *Owner->GetName());

        return;
    }

    if (bLogTagChanges)
    {
        for (const FGameplayTag& Tag : Tags)
        {
            if (HasMatchingGameplayTag(Tag))
            {
                UE_LOG(LogRTS, Log, TEXT("Removing tag %s from %s."), *Tag.ToString(), *Owner->GetName());
            }
        }
    }

    RemoveLooseGameplayTags(Tags);
    RemoveMinimalReplicationGameplayTags(Tags);
}

float URTSAbilitySystemComponent::GetAbilityRange(TSubclassOf<URTSGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return 0.0f;
    }

    for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
    {
        URTSGameplayAbility* AbilityCDO = Ability->GetDefaultObject<URTSGameplayAbility>();
        if (Spec.Ability == AbilityCDO)
        {
            return AbilityCDO->GetRange(Spec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo());
        }
    }

    return 0.0f;
}

void URTSAbilitySystemComponent::GetAutoOrders_Implementation(TArray<FRTSOrderTypeWithIndex>& OutAutoOrders)
{
    TArray<TSubclassOf<UGameplayAbility>> BasicAttackAbilities = URTSAbilitySystemHelper::GetBasicAttackAbilities(this);
    TArray<TSubclassOf<UGameplayAbility>> InitialAndUnlockableAbilities = GetInitialAndUnlockableAbilities();

    for (int32 Index = 0; Index < InitialAndUnlockableAbilities.Num(); ++Index)
    {
        TSubclassOf<UGameplayAbility> AbilityType = InitialAndUnlockableAbilities[Index];

        if (AbilityType == nullptr)
        {
            continue;
        }

        URTSGameplayAbility* Ability = AbilityType->GetDefaultObject<URTSGameplayAbility>();
        if (Ability == nullptr)
        {
            continue;
        }

        if (Ability->GetTargetType() != ERTSTargetType::PASSIVE && !BasicAttackAbilities.Contains(AbilityType))
        {
            OutAutoOrders.Add(FRTSOrderTypeWithIndex(UseAbilityOrder, Index));
        }
    }
}

void URTSAbilitySystemComponent::AddTag(const FGameplayTag Tag)
{
    FGameplayTagContainer Container;
    Container.AddTag(Tag);

    AddTags(Container);
}

void URTSAbilitySystemComponent::RemoveTag(const FGameplayTag Tag)
{
    FGameplayTagContainer Container;
    Container.AddTag(Tag);

    RemoveTags(Container);
}

void URTSAbilitySystemComponent::OnOwningPlayerTagAddedOrRemoved(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount)
    {
        AddTag(Tag);

        // Increase ability level.
        TSubclassOf<UGameplayAbility> AbilityToLevelUp = IncreaseAbilityLevelPlayerTags.FindRef(Tag);

        if (AbilityToLevelUp != nullptr)
        {
            IncreaseAbilityLevel(AbilityToLevelUp, false);
        }
    }
    else
    {
        RemoveTag(Tag);
    }
}

void URTSAbilitySystemComponent::FindAttributeSetStatusTags(FGameplayTagContainer& TagContainer)
{
    for (FAttributeDefaults AttributeDefaults : DefaultStartingData)
    {
        TSubclassOf<UAttributeSet> AttributeSetClass = AttributeDefaults.Attributes;
        if (IsValid(AttributeSetClass))
        {
            URTSAttributeSet* AttributeSet = AttributeSetClass->GetDefaultObject<URTSAttributeSet>();
            if (IsValid(AttributeSet))
            {
                AttributeSet->GetDefaultStatusTags(TagContainer);
            }
        }
    }
}

void URTSAbilitySystemComponent::FindComponentStatusTags(FGameplayTagContainer& TagContainer)
{
    AActor* Owner = GetOwner();
    UClass* TagProviderInterfaceClass = URTSInitialStatusTagsProvider::StaticClass();

    if (Owner->GetClass()->ImplementsInterface(TagProviderInterfaceClass))
    {
        IRTSInitialStatusTagsProvider::Execute_GetInitialStatusTags(Owner, TagContainer);
    }

    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->GetClass()->ImplementsInterface(TagProviderInterfaceClass))
        {
            IRTSInitialStatusTagsProvider::Execute_GetInitialStatusTags(Component, TagContainer);
        }
    }
}

void URTSAbilitySystemComponent::ReceivedCollectedXP(float OldCollectedXP)
{
    // Notify listers.
    NotifyOnCollectedXPChanged(OldCollectedXP, CollectedXP);

    UpdateLevel();
}

void URTSAbilitySystemComponent::ReceivedAbilityPoints(int32 OldAbilityPoints)
{
    NotifyOnAbilityPointsChanged(OldAbilityPoints, AbilityPoints);
}

void URTSAbilitySystemComponent::NotifyOnCollectedXPChanged(float OldCollectedXP, float NewCollectedXP)
{
    OnCollectedXPChanged.Broadcast(OldCollectedXP, NewCollectedXP);
}

void URTSAbilitySystemComponent::NotifyOnAbilityPointsChanged(int32 OldAbilityPoints, int32 NewAbilityPoints)
{
    // Notify listeners.
    OnAbilityPointsChanged.Broadcast(OldAbilityPoints, NewAbilityPoints);
}

void URTSAbilitySystemComponent::UpdateLevel()
{
    int32 NewLevel;

    float ValueAtLevel = 0;

    // TODO: Remove magic number
    for (NewLevel = 1; NewLevel <= 20; ++NewLevel)
    {
        ValueAtLevel += XPPerLevel.GetValueAtLevel(NewLevel);
        if (CollectedXP < ValueAtLevel)
        {
            break;
        }
    }

    if (NewLevel != Level)
    {
        SetLevel(NewLevel);
    }
}

void URTSAbilitySystemComponent::AbilityEndedCallback(const FAbilityEndedData& AbilityEndedData)
{
    OnGameplayAbilityEnded.Broadcast(AbilityEndedData.AbilityThatEnded->GetClass(), AbilityEndedData.AbilitySpecHandle,
                                     AbilityEndedData.bWasCancelled);
}

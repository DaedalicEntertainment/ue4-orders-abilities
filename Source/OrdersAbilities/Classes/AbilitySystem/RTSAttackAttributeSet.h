#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/RTSAttributeSet.h"
#include "RTSAttackAttributeSet.generated.h"

/**
 * Contains attributes about the attack of a unit or building.
 */
UCLASS(BlueprintType)
class ORDERSABILITIES_API URTSAttackAttributeSet : public URTSAttributeSet
{
    GENERATED_BODY()

    //  YOU ARE NOT ALLOWED TO CHANGE THE ATTRIBUTE VALUES!
    //
    //  This attributes are only PUBLIC because of the 'DEFINE_ATTRIBUTE_CAPTUREDEF' macro that is used when
    //  implementing a 'UGameplayEffectExecutionCalculation'.This attribute values should only be changed using
    //  GameplayEffects.
public:
    static const float MAX_DAMAGE;
    static const float MAX_COOLDOWN;
    static const float MAX_RANGE;
    static const float MAX_OUTGOING_DAMAGE_MULTIPLIER;

    /** Default attack damage. */
    UPROPERTY(Category = "Attributes|Attack", BlueprintReadOnly, ReplicatedUsing = OnRep_Damage)
    float Damage;
    UFUNCTION()
    void OnRep_Damage();
    static const FGameplayAttribute& DamageAttribute();

    /** Time before the attack can be used again, in seconds. */
    UPROPERTY(Category = "Attributes|Attack", BlueprintReadOnly, ReplicatedUsing = OnRep_Cooldown)
    float Cooldown;
    UFUNCTION()
    void OnRep_Cooldown();
    static const FGameplayAttribute& CooldownAttribute();

    /** Attack range, in cm. */
    UPROPERTY(Category = "Attributes|Attack", BlueprintReadOnly, ReplicatedUsing = OnRep_Range)
    float Range;
    UFUNCTION()
    void OnRep_Range();
    static const FGameplayAttribute& RangeAttribute();

    /**
     * The OutgoingDamageMultiplier of a target is multiplied with incoming damage to modify the loss of hit points of
     * the attacked unit.
     */
    UPROPERTY(Category = "Attributes|Attack", BlueprintReadOnly, ReplicatedUsing = OnRep_OutgoingDamageMultiplier)
    float OutgoingDamageMultiplier;
    UFUNCTION()
    void OnRep_OutgoingDamageMultiplier();
    static const FGameplayAttribute& OutgoingDamageMultiplierAttribute();

    //~ Begin UAttributeSet Interface
    virtual bool ShouldInitProperty(bool FirstInit, UProperty* PropertyToInit) const override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    //~ End UAttributeSet Interface

    //~ Begin URTSAttributeSet Interface
    virtual void PostInitializeProperties(bool bInitialInit) override;
    virtual void GetDefaultStatusTags(FGameplayTagContainer& OutStatusTags) const override;
    //~ End URTSAttributeSet Interface
};

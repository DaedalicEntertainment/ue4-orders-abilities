#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Abilities/GameplayAbility.h"
#include "RTSOrderPreview.generated.h"

/**Previews the execution of orders and abilities.*/
UCLASS()
class ORDERSABILITIES_API ARTSOrderPreview : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(Category = RTS, BlueprintPure)
    TSubclassOf<UGameplayAbility> GetInstigatorAbility() const;

    UFUNCTION(Category = RTS, BlueprintCallable)
    void SetInstigatorAbility(TSubclassOf<UGameplayAbility> Ability);

    UFUNCTION(Category = RTS, BlueprintImplementableEvent, meta = (DisplayName = "OnInstigatorAbilityInitialized"))
    void ReceiveOnInstigatorAbilityInitialized();

private:
    /** InstigatorAbility will be used to get information, like the range of an ability, for the OrderPreview */
    UPROPERTY(Category = RTS, BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
    TSubclassOf<UGameplayAbility> InstigatorAbility;
};

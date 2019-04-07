#include "Orders/RTSOrderPreview.h"


TSubclassOf<UGameplayAbility> ARTSOrderPreview::GetInstigatorAbility() const
{
    return InstigatorAbility;
}

void ARTSOrderPreview::SetInstigatorAbility(TSubclassOf<UGameplayAbility> Ability)
{
    InstigatorAbility = Ability;

    ReceiveOnInstigatorAbilityInitialized();
}

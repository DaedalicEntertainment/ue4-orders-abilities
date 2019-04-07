#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Orders/RTSOrderPreviewSnapMode.h"
#include "Orders/RTSOrderPreview.h"
#include "RTSOrderPreviewData.generated.h"

/** Specifies how to show previews while selecting an order target. */
USTRUCT(BlueprintType)
struct ORDERSABILITIES_API FRTSOrderPreviewData
{
    GENERATED_USTRUCT_BODY()

public:
    /** Actor the preview is shown for. */
    UPROPERTY()
    AActor* Owner;

    /** Actor previewing the order. */
    UPROPERTY()
    AActor* Preview;

    /** Gets the class of actors to spawn as preview for an order. */
    TSubclassOf<ARTSOrderPreview> GetOrderPreviewClass();

    /** Gets whether and how to snap order previews. */
    ERTSOrderPreviewSnapMode GetSnapMode();

private:
    /** Class of actors to spawn as preview for an order. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<ARTSOrderPreview> OrderPreviewClass;

    /** Whether and how to snap order previews. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS", meta = (AllowPrivateAccess = "true"))
    ERTSOrderPreviewSnapMode SnapMode;
};

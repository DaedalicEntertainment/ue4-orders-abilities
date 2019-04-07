#pragma once

#include "CoreMinimal.h"

#include "RTSOrderPreviewSnapMode.generated.h"

/** Whether and how to snap order previews. */
UENUM(BlueprintType)
enum class ERTSOrderPreviewSnapMode : uint8
{
    /** Don't snap. */
    ORDERPREVIEW_DontSnap,

    /** Snap to the actor the preview is shown for, and rotate towards the cursor position. */
    ORDERPREVIEW_SnapToOwner,

    /** Snap to the cursor position. */
    ORDERPREVIEW_SnapToCursor
};

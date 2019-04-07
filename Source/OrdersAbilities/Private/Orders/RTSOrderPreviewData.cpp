#include "Orders/RTSOrderPreviewData.h"


TSubclassOf<ARTSOrderPreview> FRTSOrderPreviewData::GetOrderPreviewClass()
{
    return OrderPreviewClass;
}

ERTSOrderPreviewSnapMode FRTSOrderPreviewData::GetSnapMode()
{
    return SnapMode;
}

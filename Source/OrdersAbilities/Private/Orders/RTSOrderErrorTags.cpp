#include "Orders/RTSOrderErrorTags.h"


bool FRTSOrderErrorTags::IsEmpty() const
{
    return MissingTags.IsEmpty() && BlockingTags.IsEmpty() && ErrorTags.IsEmpty();
}

FString FRTSOrderErrorTags::ToString() const
{
    FString s;
    s += TEXT("Missing Tags: ");
    s += MissingTags.ToString();

    s += TEXT(", Blocking Tags: ");
    s += BlockingTags.ToString();

    s += TEXT(", Error Tags: ");
    s += ErrorTags.ToString();

    return s;
}

#include "Orders/RTSOrderTypeWithIndex.h"

#include "GameFramework/Actor.h"

#include "Orders/RTSOrder.h"


FRTSOrderTypeWithIndex::FRTSOrderTypeWithIndex()
{
    OrderType = nullptr;
    Index = -1;
}

FRTSOrderTypeWithIndex::FRTSOrderTypeWithIndex(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex)
    : OrderType(InOrderType)
    , Index(InIndex)
{
}

FString FRTSOrderTypeWithIndex::ToString() const
{
    FString s;

    if (OrderType != nullptr)
    {
        if (!OrderType.IsValid())
        {
            OrderType.LoadSynchronous();
        }

        s += OrderType->GetName();
    }
    else
    {
        s += TEXT("Unknown Order");
    }

    s += TEXT(" (");

    if (Index >= 0)
    {
        s += TEXT(", Index: ");
        s += FString::FromInt(Index);
    }

    s += TEXT(")");

    return s;
}

bool FRTSOrderTypeWithIndex::operator==(const FRTSOrderTypeWithIndex& Other) const
{
    return OrderType == Other.OrderType && Index == Other.Index;
}

bool FRTSOrderTypeWithIndex::operator!=(const FRTSOrderTypeWithIndex& Other) const
{
    return !(*this == Other);
}

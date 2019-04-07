#include "Orders/RTSBlackboardHelper.h"

#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"


const FName URTSBlackboardHelper::BLACKBOARD_KEY_ORDER_TYPE = TEXT("Order_OrderType");
const FName URTSBlackboardHelper::BLACKBOARD_KEY_LOCATION = TEXT("Order_Location");
const FName URTSBlackboardHelper::BLACKBOARD_KEY_TARGET = TEXT("Order_Target");
const FName URTSBlackboardHelper::BLACKBOARD_KEY_INDEX = TEXT("Order_Index");
const FName URTSBlackboardHelper::BLACKBOARD_KEY_RANGE = TEXT("Order_Range");
const FName URTSBlackboardHelper::BLACKBOARD_KEY_HOME_LOCATION = TEXT("Order_HomeLocation");

TSubclassOf<URTSOrder> URTSBlackboardHelper::GetBlackboardOrderType(UBTNode* NodeOwner)
{
    UBlackboardComponent* BlackboardComp = UBTFunctionLibrary::GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsClass(BLACKBOARD_KEY_ORDER_TYPE) : nullptr;
}

FVector URTSBlackboardHelper::GetBlackboardOrderLocation(UBTNode* NodeOwner)
{
    UBlackboardComponent* BlackboardComp = UBTFunctionLibrary::GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsVector(BLACKBOARD_KEY_LOCATION) : FVector::ZeroVector;
}

AActor* URTSBlackboardHelper::GetBlackboardOrderTarget(UBTNode* NodeOwner)
{
    UBlackboardComponent* BlackboardComp = UBTFunctionLibrary::GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(BLACKBOARD_KEY_TARGET)) : nullptr;
}

int32 URTSBlackboardHelper::GetBlackboardOrderIndex(UBTNode* NodeOwner)
{
    UBlackboardComponent* BlackboardComp = UBTFunctionLibrary::GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsInt(BLACKBOARD_KEY_INDEX) : 0;
}

float URTSBlackboardHelper::GetBlackboardOrderRange(UBTNode* NodeOwner)
{
    UBlackboardComponent* BlackboardComp = UBTFunctionLibrary::GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsFloat(BLACKBOARD_KEY_RANGE) : 0.0f;
}

FVector URTSBlackboardHelper::GetBlackboardOrderHomeLocation(UBTNode* NodeOwner)
{
    UBlackboardComponent* BlackboardComp = UBTFunctionLibrary::GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsVector(BLACKBOARD_KEY_HOME_LOCATION) : FVector::ZeroVector;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTreeServices/BTService_InRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_InRange::UBTService_InRange(const FObjectInitializer& ObjectInitializer)
{
    NodeName = FString("In Range");
	Range = 200.0f;
	bNotifyTick = false;
	bTickIntervals = true;
	bNotifyCeaseRelevant = false;
	bNotifyBecomeRelevant = true;

    IsInRangeKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_InRange, IsInRangeKey));
    ActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_InRange, ActorKey), AActor::StaticClass());
}

void UBTService_InRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (AIController == nullptr || BlackboardComp == nullptr) return;

    APawn* ControlledPawn = AIController->GetPawn();
    if (ControlledPawn == nullptr) return;

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(ActorKey.SelectedKeyName));
    float Distance = ControlledPawn->GetDistanceTo(TargetActor);
    BlackboardComp->SetValueAsBool(IsInRangeKey.SelectedKeyName, Distance <= Range);
}

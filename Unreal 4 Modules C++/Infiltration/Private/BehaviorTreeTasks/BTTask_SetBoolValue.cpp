// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTreeTasks/BTTask_SetBoolValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBoolValue::UBTTask_SetBoolValue(const FObjectInitializer& ObjectInitializer)
{
    /* Friendly name for this task. */
    NodeName = FString("Set Bool Value");

     /* Setup of key filters for this task */
     BoolKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetBoolValue, BoolKey));
}

EBTNodeResult::Type UBTTask_SetBoolValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    /* Assert that the owner component does indeed have a blackboard and fail the task if it does not. */
    UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr) return EBTNodeResult::Failed;

    BlackboardComponent->SetValueAsBool(BoolKey.SelectedKeyName, bValueToSet);

    return EBTNodeResult::Succeeded;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTreeTasks/BTTask_SelectNextPathPoint.h"
#include "Actors/PathPoints.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectNextPathPoint::UBTTask_SelectNextPathPoint(const FObjectInitializer& ObjectInitializer)
{
    /* Friendly name for this task. */
    NodeName = FString("Select Next Point");

    /* Setup of key filters for this task */
    IndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SelectNextPathPoint, IndexKey));
    NextPointKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SelectNextPathPoint, NextPointKey));
    PathPointsKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SelectNextPathPoint, PathPointsKey), APathPoints::StaticClass());
}

EBTNodeResult::Type UBTTask_SelectNextPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    /* Assert that the owner component does indeed have a blackboard and fail the task if it does not. */
    UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr) return EBTNodeResult::Failed;

    /* Attempts to get the PathPoints being held as a blackboard key. If the value is null or the points is empty, simply return success
    (noting that Failed would stop the BT's execution; a PathPoints many simply have not been set.) */
    APathPoints* PathPoints = Cast<APathPoints>(BlackboardComponent->GetValueAsObject(PathPointsKey.SelectedKeyName));
    if (PathPoints == nullptr) return EBTNodeResult::Succeeded;
    if (PathPoints->Points.Num() == 0) return EBTNodeResult::Succeeded;

    /* Get the current index from the blackboard. Set back down to zero if it exceeds the length of the points.
    Update the respective keys with the new values. */
    int32 Index = BlackboardComponent->GetValueAsInt(IndexKey.SelectedKeyName);
    if (Index >= PathPoints->Points.Num()) Index = 0;

    BlackboardComponent->SetValueAsVector(NextPointKey.SelectedKeyName, PathPoints->MakeWorldTransformFromPoint(Index));
    BlackboardComponent->SetValueAsInt(IndexKey.SelectedKeyName, Index + 1);

    return EBTNodeResult::Succeeded;
}

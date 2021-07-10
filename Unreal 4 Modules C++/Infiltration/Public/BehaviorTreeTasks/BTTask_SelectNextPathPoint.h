// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectNextPathPoint.generated.h"

/* A task which selects the next point on a PathPoints object and updates the Next Point key with that point.  */
UCLASS()
class INFILTRATION_API UBTTask_SelectNextPathPoint : public UBTTaskNode
{
	GENERATED_BODY()

/* --- VARIABLES --- */
protected:
	/* --- KEYS --- */

	/* Blackboard Key Selector for path points index. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector IndexKey;

	/* Blackboard Key Selector for actual patrol path location
	(this is inteded to be used by a Move To node) */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector NextPointKey;

	/* Blackboard Key Selector for the path points actor. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector PathPointsKey;

/* --- FUNCTIONS --- */
public:
	/** 
	 * Boilerplate constructor for filtering key types.
	 * @param ObjectInitializer - Unused.
	 */
	UBTTask_SelectNextPathPoint(const FObjectInitializer& ObjectInitializer);

	/** 
	 * Override of node which is executed by a behaviour tree.
	 * @param OwnerComp - The behaviour tree which "owns" this task. Used to find get the Blackboard, controller and controlled pawn.
	 * @param NodeMemory - Memory allocation used by this node. Unused.
	 * @return What result ExecuteTask finished with.
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

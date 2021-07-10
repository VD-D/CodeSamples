// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetBoolValue.generated.h"

/* Force sets a bool value on the Blackboard. */
UCLASS()
class INFILTRATION_API UBTTask_SetBoolValue : public UBTTaskNode
{
	GENERATED_BODY()
/* --- VARIABLES --- */
protected:
	/* Blackboard Key Selector bool value. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector BoolKey;

	/* Boolean value will be set to this value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blackboard)
	bool bValueToSet;

/* --- FUNCTIONS --- */
public:
	/** 
	 * Boilerplate constructor for filtering key types.
	 * @param ObjectInitializer - Unused.
	 */
	UBTTask_SetBoolValue(const FObjectInitializer& ObjectInitializer);

	/** 
	 * Override of node which is executed by a behaviour tree.
	 * @param OwnerComp - The behaviour tree which "owns" this task. Used to find get the Blackboard, controller and controlled pawn.
	 * @param NodeMemory - Memory allocation used by this node. Unused.
	 * @return What result ExecuteTask finished with.
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

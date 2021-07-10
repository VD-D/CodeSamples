// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteCombatInterface.generated.h"

/* Task which executes the combat interface on its parent controller and controlled pawn. */
UCLASS()
class INFILTRATION_API UBTTask_ExecuteCombatInterface : public UBTTaskNode
{
	GENERATED_BODY()
/* --- VARIABLES --- */
protected:
	/* Key selector specifying the target to attack. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector ActorKey;

	/* Amount of time required until this task can be called again */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Interface Invoker")
	float Delay;

	/* Variation in delay, i.e. FinalDelay = Delay + or - DelayVariation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Interface Invoker")
	float DelayVariation;

private:
	bool bIsOnCooldown;

/* --- FUNCTIONS --- */
public:
	/** 
	 * Boilerplate constructor for filtering key types.
	 * @param ObjectInitializer - Unused.
	 */
	UBTTask_ExecuteCombatInterface(const FObjectInitializer& ObjectInitializer);

	/** 
	 * Override of node which is executed by a behaviour tree.
	 * @param OwnerComp - The behaviour tree which "owns" this task. Used to find get the Blackboard, controller and controlled pawn.
	 * @param NodeMemory - Memory allocation used by this node. Unused.
	 * @return What result ExecuteTask finished with.
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	/**
	 * Resets the cooldown of the timer.
	 */
	UFUNCTION()
	void ResetCooldown();
};

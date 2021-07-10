// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_InRange.generated.h"

/* Updates a blackboard key with whether or not the owning actor is in range of the target actor. */
UCLASS()
class INFILTRATION_API UBTService_InRange : public UBTService
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Interface Invoker")
	float Range;

	/* Blackboard Key Selector for whether not is in range. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector IsInRangeKey;
	
	/* Blackboard Key Selector for actor to test range against. */
	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector ActorKey;

/* --- FUNCTIONS --- */
protected:
	/**
	 * Constructor.
	 * @param ObjectInitializer - Unused.
	 */
	UBTService_InRange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 
	 * Called by a behaviour tree. Tries to set NewGait on the owning actor. Will do nothing if 
	 * the controlled pawn cannot be cast to ALocomotionCharacter.
	 * @param OwnerComp - The behaviour tree which "owns" this component.
	 * @param NodeMemory - The amount of memory allocated to this node. Unused.
	 */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

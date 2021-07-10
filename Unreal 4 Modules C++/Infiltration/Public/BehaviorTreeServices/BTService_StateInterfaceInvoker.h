// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Interfaces/InfiltrationStateInterface.h"
#include "BTService_StateInterfaceInvoker.generated.h"

/* Service which attempts to execute the UpdateState from InfiltrationStateInterface on both the AI controller and controlled pawn. */
UCLASS()
class INFILTRATION_API UBTService_StateInterfaceInvoker : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Interface Invoker")
	EInfiltrationState StateToSet;

protected:
	/**
	 * Constructor.
	 * @param ObjectInitializer - Unused.
	 */
	UBTService_StateInterfaceInvoker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 
	 * Called by a behaviour tree. Tries to set NewGait on the owning actor. Will do nothing if 
	 * the controlled pawn cannot be cast to ALocomotionCharacter.
	 * @param OwnerComp - The behaviour tree which "owns" this component.
	 * @param NodeMemory - The amount of memory allocated to this node. Unused.
	 */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

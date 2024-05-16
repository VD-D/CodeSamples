// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BTTask_Autobattler.h"
#include "BTTask_ResetMovementState.generated.h"

/**
 * Halts movement and sets IsMovingToTarget false for AI controller.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTTask_ResetMovementState : public UBTTask_Autobattler
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// BT TASK INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Halts movement and sets IsMovingToTarget
	 * @param OwnerController Used to set state.
	 * @param ControlledCharacter Unused.
	 */
	virtual EBTNodeResult::Type ExecuteAutobattlerTask(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter) override;
};

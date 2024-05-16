// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BTTask_Autobattler.h"
#include "BTTask_ExecuteSkill.generated.h"

/**
 * Task responsible for executing an autobattler character's skill.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTTask_ExecuteSkill : public UBTTask_Autobattler
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// BT TASK INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Constructor.
	 */
	UBTTask_ExecuteSkill();

	/**
	 * Attempts to execute autobattler skill on AI Controller.
	 * @param OwnerComp Unused.
	 * @param OwnerController Wants to execute skill on.
	 * @param ControlledCharacter Unused.
	 * @return Success if skill executes.
	 */
	virtual EBTNodeResult::Type ExecuteAutobattlerTask(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter) override;
};

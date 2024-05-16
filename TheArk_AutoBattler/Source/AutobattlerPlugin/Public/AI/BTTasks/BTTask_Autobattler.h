// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Autobattler.generated.h"

class AAutobattlerAIController;
class AAutobattlerCharacter;

/**
 * Base class for all autobattler tasks to inherit from.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTTask_Autobattler : public UBTTaskNode
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// BT TASK INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 */
	UBTTask_Autobattler();
	
	/**
	 * Executes Autobattler equivalent of this task.
	 * @param OwnerComp Gets owner character and AI controller.
	 * @param NodeMemory Unused.
	 * @return Task success.
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/**
	 * Intended to be overriden by Autobattler sub-tasks.
	 * @param OwnerController AI owner controller.
	 * @param ControlledCharacter Character controlled by AI controller.
	 * @return Task success.
	 */
	virtual EBTNodeResult::Type ExecuteAutobattlerTask(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter);

	/**
	 * Gets description of this task.
	 * @return Description of this task.
	 */
	virtual FString GetStaticDescription() const override;
};

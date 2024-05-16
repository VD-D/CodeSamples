// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/BTDecorators/BTDecorator_Autobattler.h"
#include "BTDecorator_ShouldEverUpdate.generated.h"

/**
 * Decorator which checks if the AI should ever update.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTDecorator_ShouldEverUpdate : public UBTDecorator_Autobattler
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// BT DECORATOR INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Returns whether the AI should ever update
	 * @param OwnerComp Unused.
	 * @param AIController Used to check condition.
	 * @param ControlledCharacter
	 * @return Whether the AI should ever update.
	 */
	virtual bool AutobattlerCalculateDecoratorCondition(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* AIController, AAutobattlerCharacter* ControlledCharacter) const override;
};

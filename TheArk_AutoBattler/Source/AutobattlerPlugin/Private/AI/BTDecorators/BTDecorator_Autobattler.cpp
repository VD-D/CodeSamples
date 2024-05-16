// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTDecorators/BTDecorator_Autobattler.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"

bool UBTDecorator_Autobattler::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAutobattlerAIController* AIOwner = Cast<AAutobattlerAIController>(OwnerComp.GetAIOwner());
	AAutobattlerCharacter* Pawn = Cast<AAutobattlerCharacter>(AIOwner->GetPawn());
    return AutobattlerCalculateDecoratorCondition(OwnerComp, AIOwner, Pawn);
}

bool UBTDecorator_Autobattler::AutobattlerCalculateDecoratorCondition(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* AIController, AAutobattlerCharacter* ControlledCharacter) const
{
    // Dummy implementation.
    return true;
}

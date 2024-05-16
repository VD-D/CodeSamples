// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTasks/BTTask_Autobattler.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"

UBTTask_Autobattler::UBTTask_Autobattler()
{
    NodeName = "Autobattler Task";
}

EBTNodeResult::Type UBTTask_Autobattler::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAutobattlerAIController* AIOwner = Cast<AAutobattlerAIController>(OwnerComp.GetAIOwner());
	AAutobattlerCharacter* Pawn = Cast<AAutobattlerCharacter>(AIOwner->GetPawn());

    return ExecuteAutobattlerTask(OwnerComp, AIOwner, Pawn);
}

EBTNodeResult::Type UBTTask_Autobattler::ExecuteAutobattlerTask(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter)
{
    return EBTNodeResult::Type::Succeeded;
}

FString UBTTask_Autobattler::GetStaticDescription() const
{
    return FString::Printf(TEXT("%s"), *UBTTaskNode::GetStaticDescription());
}

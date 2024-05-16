// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTServices/BTService_Autobattler.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"

void UBTService_Autobattler::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAutobattlerAIController* AIOwner = Cast<AAutobattlerAIController>(OwnerComp.GetAIOwner());
	AAutobattlerCharacter* Pawn = Cast<AAutobattlerCharacter>(AIOwner->GetPawn());
    TickAutobattlerNode(OwnerComp, AIOwner, Pawn, DeltaSeconds);
}

void UBTService_Autobattler::TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds)
{
    // Dummy implementation
}

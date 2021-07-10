// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTreeServices/BTService_StateInterfaceInvoker.h"
#include "AIController.h"
#include "Libraries/InfiltrationUtilityLibrary.h"

UBTService_StateInterfaceInvoker::UBTService_StateInterfaceInvoker(const FObjectInitializer& ObjectInitializer)
{
    NodeName = FString("Invoke State Interface");
	StateToSet = EInfiltrationState::Idle;
	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyCeaseRelevant = false;
	bNotifyBecomeRelevant = true;
}

void UBTService_StateInterfaceInvoker::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr) return;

    APawn* ControlledPawn = AIController->GetPawn();

    IInfiltrationStateInterface* AI_I = Cast<IInfiltrationStateInterface>(AIController);
    IInfiltrationStateInterface* Pawn_I = Cast<IInfiltrationStateInterface>(ControlledPawn);

    if (AI_I != nullptr) AI_I->Execute_UpdateState(AIController, StateToSet);
    if (Pawn_I != nullptr) Pawn_I->Execute_UpdateState(ControlledPawn, StateToSet);
}

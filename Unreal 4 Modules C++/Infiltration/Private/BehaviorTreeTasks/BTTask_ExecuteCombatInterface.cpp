// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTreeTasks/BTTask_ExecuteCombatInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/CombatInterface.h"

UBTTask_ExecuteCombatInterface::UBTTask_ExecuteCombatInterface(const FObjectInitializer& ObjectInitializer)
{
    /* Friendly name for this task. */
    NodeName = FString("Execute Combat Interface");
    Delay = 1.5f;
    DelayVariation = 0.0f;

    /* Setup of key filters for this task */
    ActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ExecuteCombatInterface, ActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_ExecuteCombatInterface::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (GetWorld() == nullptr) return EBTNodeResult::Type::Failed;
    if (bIsOnCooldown) return EBTNodeResult::Type::Succeeded;

    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (AIController == nullptr || BlackboardComp == nullptr) return EBTNodeResult::Type::Failed;

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(ActorKey.SelectedKeyName));
    if (TargetActor == nullptr) return EBTNodeResult::Type::Succeeded;

    ICombatInterface* AI_I = Cast<ICombatInterface>(AIController);
    if (AI_I != nullptr) AI_I->Execute_Attack(AIController, TargetActor);

    APawn* ControlledPawn = AIController->GetPawn();
    if (ControlledPawn != nullptr)
    {
        ICombatInterface* CP_I = Cast<ICombatInterface>(ControlledPawn);
        if (CP_I != nullptr) CP_I->Execute_Attack(ControlledPawn, TargetActor);
    }

    if (!bIsOnCooldown) 
    {
        bIsOnCooldown = true;

        FTimerHandle CooldownTimerHandle;
        FTimerDelegate CooldownTimerDelegate;
        CooldownTimerDelegate.BindUFunction(this, FName("ResetCooldown"));

        float FinalDelay = FMath::RandRange(Delay - DelayVariation, Delay + DelayVariation);
        FinalDelay = FinalDelay < 0.0f ? 0.0f : FinalDelay;
        GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, CooldownTimerDelegate, FinalDelay, false);
    }

    return EBTNodeResult::Type::Succeeded;
}

void UBTTask_ExecuteCombatInterface::ResetCooldown()
{
    bIsOnCooldown = false;
}

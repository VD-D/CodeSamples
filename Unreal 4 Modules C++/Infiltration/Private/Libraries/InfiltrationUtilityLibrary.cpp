// Fill out your copyright notice in the Description page of Project Settings.

#include "Libraries/InfiltrationUtilityLibrary.h"
#include "Components/InfiltrationAIComponent.h"
#include "Controllers/AIInfiltrationController.h"

#include "BehaviorTree/BlackboardComponent.h"

void UInfiltrationUtilityLibrary::SetFloatValuesOnControllerBB(AAIInfiltrationController * AIInfiltrationController, const TMap<FName, float>& ValuesToSet)
{
    if (AIInfiltrationController == nullptr) return;
    
    UBlackboardComponent* BlackboardComp = AIInfiltrationController->GetBlackboardComponent();
    if (BlackboardComp == nullptr) return;

    for (auto ValueToSet : ValuesToSet)
    {
        BlackboardComp->SetValueAsFloat(ValueToSet.Key, ValueToSet.Value);
    }
}

void UInfiltrationUtilityLibrary::SetVectorValuesOnControllerBB(AAIInfiltrationController * AIInfiltrationController, const TMap<FName, FVector>& ValuesToSet)
{
    if (AIInfiltrationController == nullptr) return;

    UBlackboardComponent* BlackboardComp = AIInfiltrationController->GetBlackboardComponent();
    if (BlackboardComp == nullptr) return;

    for (auto ValueToSet : ValuesToSet)
    {
        BlackboardComp->SetValueAsVector(ValueToSet.Key, ValueToSet.Value);
    }
}

void UInfiltrationUtilityLibrary::SetBoolValuesOnControllerBB(AAIInfiltrationController * AIInfiltrationController, const TMap<FName, bool>& ValuesToSet)
{
    if (AIInfiltrationController == nullptr) return;

    UBlackboardComponent* BlackboardComp = AIInfiltrationController->GetBlackboardComponent();
    if (BlackboardComp == nullptr) return;

    for (auto ValueToSet : ValuesToSet)
    {
        BlackboardComp->SetValueAsBool(ValueToSet.Key, ValueToSet.Value);
    }
}

void UInfiltrationUtilityLibrary::SetStringValuesOnControllerBB(AAIInfiltrationController * AIInfiltrationController, const TMap<FName, FString>& ValuesToSet)
{
    if (AIInfiltrationController == nullptr) return;

    UBlackboardComponent* BlackboardComp = AIInfiltrationController->GetBlackboardComponent();
    if (BlackboardComp == nullptr) return;

    for (auto ValueToSet : ValuesToSet)
    {
        BlackboardComp->SetValueAsString(ValueToSet.Key, ValueToSet.Value);
    }
}

void UInfiltrationUtilityLibrary::SetObjectValuesOnControllerBB(AAIInfiltrationController * AIInfiltrationController, const TMap<FName, UObject*>& ValuesToSet)
{
    if (AIInfiltrationController == nullptr) return;

    UBlackboardComponent* BlackboardComp = AIInfiltrationController->GetBlackboardComponent();
    if (BlackboardComp == nullptr) return;

    for (auto ValueToSet : ValuesToSet)
    {
        BlackboardComp->SetValueAsObject(ValueToSet.Key, ValueToSet.Value);
    }
}

UInfiltrationAIComponent * UInfiltrationUtilityLibrary::GetInfiltrationAIComponent(AActor * Owner)
{
    if (Owner == nullptr) return nullptr;

    APawn* OwnerPawn = Cast<APawn>(Owner);
    if (OwnerPawn != nullptr) return Owner->FindComponentByClass<UInfiltrationAIComponent>();
    return nullptr;
}

AAIInfiltrationController * UInfiltrationUtilityLibrary::GetInfiltrationAIController(AActor * Owner)
{
    if (Owner == nullptr) return nullptr;

    APawn* OwnerPawn = Cast<APawn>(Owner);
    if (OwnerPawn != nullptr) return Cast<AAIInfiltrationController>(OwnerPawn->GetController());
    return nullptr;
}

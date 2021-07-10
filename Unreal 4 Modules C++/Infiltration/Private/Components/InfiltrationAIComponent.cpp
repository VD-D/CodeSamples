// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InfiltrationAIComponent.h"
#include "Libraries/InfiltrationUtilityLibrary.h"

UInfiltrationAIComponent::UInfiltrationAIComponent()
{
	/* Tick not required */
	PrimaryComponentTick.bCanEverTick = false;

	/* Reasonable defaults */
	bSetValuesOnBeginPlay = true;
}

void UInfiltrationAIComponent::GetStates_Implementation(TArray<FString>& States)
{
	States = CurrentStates;
}

bool UInfiltrationAIComponent::QueryState(AActor * Target, FString QueryString)
{
	if (Target == nullptr) return false;

	UInfiltrationAIComponent* InfiltrationAIComp = UInfiltrationUtilityLibrary::GetInfiltrationAIComponent(Target);
	if (InfiltrationAIComp == nullptr) return false;

	IInfiltrationStateInterface* I = Cast<IInfiltrationStateInterface>(InfiltrationAIComp);
	if (I != nullptr)
	{
		TArray<FString> TargetStates;
		I->Execute_GetStates(Target, TargetStates);

		if (TargetStates.Num() == 0) return false;
		else return TargetStates.Contains(QueryString);
	}
	else return false;
}

void UInfiltrationAIComponent::BeginPlay()
{
	Super::BeginPlay();
	/* If values are flagged to be set on begin play, then set them. */
	if (bSetValuesOnBeginPlay)
	{
		UInfiltrationUtilityLibrary::SetBoolValuesOnControllerBB(UInfiltrationUtilityLibrary::GetInfiltrationAIController(GetOwner()), BoolValues);
		UInfiltrationUtilityLibrary::SetFloatValuesOnControllerBB(UInfiltrationUtilityLibrary::GetInfiltrationAIController(GetOwner()), FloatValues);
		UInfiltrationUtilityLibrary::SetVectorValuesOnControllerBB(UInfiltrationUtilityLibrary::GetInfiltrationAIController(GetOwner()), VectorValues);
		UInfiltrationUtilityLibrary::SetStringValuesOnControllerBB(UInfiltrationUtilityLibrary::GetInfiltrationAIController(GetOwner()), StringValues);
	}
}

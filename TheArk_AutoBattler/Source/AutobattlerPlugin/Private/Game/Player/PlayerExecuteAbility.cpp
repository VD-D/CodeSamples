// Fill out your copyright notice in the Description page of Project Settings.

/* Class header. */
#include "Game/Player/PlayerExecuteAbility.h"

/* Autobattler includes. */
#include "Core/AutobattlerControllerComponent.h"
#include "Game/Skills/SkillImplementation.h"
#include "Game/Units/AutobattlerCharacter.h"

/* UE4 includes. */
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerExecuteAbility::APlayerExecuteAbility()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APlayerExecuteAbility::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SnapsToTarget && IsValid(SnapToActor))
	{
		SetActorLocation(SnapToActor->GetActorLocation());
	}
}

void APlayerExecuteAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerExecuteAbility, SnapToActor);
}

void APlayerExecuteAbility::ExecuteAbility(FAutobattlerPlayerAbilityData* PlayerAbilityData, UAutobattlerControllerComponent* SkillOwner, AAutobattlerCharacter* Target)
{
	if (PlayerAbilityData != nullptr && IsValid(Target))
	{
		if (PlayerAbilityData->SkillEffects.Num() > 0)
		{
			for (auto SkillEffect : PlayerAbilityData->SkillEffects)
			{
				SkillEffect->ExecuteSkill(nullptr, ESkillTargetingMode::Actor, Target, Target->GetActorLocation());
			}

			OnAbilityExecuted(SkillOwner, Target);
		}
	}
}

void APlayerExecuteAbility::OnAbilityExecuted_Implementation(UAutobattlerControllerComponent* SkillOwner, AAutobattlerCharacter* Target)
{

}

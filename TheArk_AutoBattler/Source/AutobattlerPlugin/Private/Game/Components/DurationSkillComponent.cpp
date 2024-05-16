// Copyright Juggler Games 2022 - 2024
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Components/DurationSkillComponent.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerSkill.h"
#include "Game/Units/AutobattlerCharacter.h"

UDurationSkillComponent::UDurationSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDurationSkillComponent::ConfigureTimedSkill(AAutobattlerCharacter* Target, const UAutobattlerSkill* SkillAsset, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	if (!IsValid(Target)) return;

	if (UDurationSkillComponent* NewDurationComponent = Cast<UDurationSkillComponent>(Target->AddComponentByClass(SkillAsset->DurationSkillComponentClass, false, FTransform(), false)))
	{
		NewDurationComponent->ConfigureTimedSkillInternal(SkillAsset, SkillOwner, TargetingProperties);
	}
}

void UDurationSkillComponent::ConfigureTimedSkillInternal(const UAutobattlerSkill* SkillAsset, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority || DurationHandle.IsValid()) return;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, FName("ReactivateSkill"), SkillAsset, SkillOwner, TargetingProperties);

	GetWorld()->GetTimerManager().SetTimer(DurationHandle, Delegate, FMath::Max(SkillAsset->Interval, 0.1f), true);
}

void UDurationSkillComponent::BeginDestroy()
{
	if (DurationHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(DurationHandle);
		}
	}

	Super::BeginDestroy();
}

void UDurationSkillComponent::ReactivateSkill(const UAutobattlerSkill* SkillAsset, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	if (!IsValid(SkillAsset) || !IsValid(SkillOwner))
	{
		DestroyComponent();
		return;
	}

	for (auto Effect : SkillAsset->SkillEffects)
	{
		if (!IsValid(Effect)) continue;

		Effect->ExecuteSkill(
			SkillOwner,
			TargetingProperties.TargetingMode,
			Cast<AAutobattlerCharacter>(GetOwner()),
			TargetingProperties.TargetLocation
		);
	}

	NumIntervalsExpired += 1;
	if (NumIntervalsExpired == SkillAsset->NumRepetitions) DestroyComponent();
}

// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/ExecuteSkill.h"

/* Autobattler includes. */
#include "Core/AutobattlerSettings.h"
#include "DataAssets/AutobattlerSkill.h"
#include "Game/Components/DurationSkillComponent.h"
#include "Game/Skills/AutobattlerProjectile.h"
#include "Game/Skills/SkillImplementation.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AExecuteSkill::AExecuteSkill()
{
 	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	EffectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	EffectMesh->SetupAttachment(RootComponent);
	EffectMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	EffectParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticle"));
	EffectParticle->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AExecuteSkill::InitialiseAsInstant(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties, bool DestroyOnEnd)
{
	if (HasBeenInitialised || !HasAuthority() || !IsValid(SkillImplementation)) return;
	HasBeenInitialised = true;

	ExecuteSkillList(SkillImplementation, SkillOwner, TargetingProperties);

	if (DestroyOnEnd) Destroy();
}

void AExecuteSkill::InitialiseAsProjectile(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	if (HasBeenInitialised || !HasAuthority() || !IsValid(SkillImplementation)) return;
	HasBeenInitialised = true;

	FActorSpawnParameters ProjectileSpawnParams;
	ProjectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAutobattlerProjectile* AutobattlerProjectile = GetWorld()->SpawnActor<AAutobattlerProjectile>(
		AAutobattlerProjectile::StaticClass(),
		GetActorTransform(),
		ProjectileSpawnParams
	);
	AutobattlerProjectile->InitialiseProjectile(SkillImplementation, SkillOwner, TargetingProperties);
}

void AExecuteSkill::InitialiseAsEffect(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	if (HasBeenInitialised || !HasAuthority() || !IsValid(SkillImplementation)) return;
	HasBeenInitialised = true;

	UpdateEffectVisuals(SkillImplementation->SkillMesh, SkillImplementation->SkillParticleEffect);
	ExecuteSkillList(SkillImplementation, SkillOwner, TargetingProperties);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OnEffectEnd"));

	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, SkillImplementation->EffectTimeout, false);
}

void AExecuteSkill::ExecuteSkillList(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	const UAutobattlerSettings* Settings = GetDefault<UAutobattlerSettings>();
	TEnumAsByte<ECollisionChannel> CharacterCollisionChannel = Settings != nullptr ? Settings->CharacterCollisionChannel : TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Pawn);

	bool ShouldExecuteSkillList = true;
	if (SkillImplementation->HasDuration)
	{
		ShouldExecuteSkillList = SkillImplementation->ActivatesImmediately;
	}

	for (auto Effect : SkillImplementation->SkillEffects)
	{
		if (!IsValid(Effect)) continue;

		if (ShouldExecuteSkillList) Effect->ExecuteSkill(
			SkillOwner,
			TargetingProperties.TargetingMode,
			TargetingProperties.TargetCharacter,
			TargetingProperties.TargetLocation
		);

		if (SkillImplementation->HasDuration) UDurationSkillComponent::ConfigureTimedSkill(TargetingProperties.TargetCharacter, SkillImplementation, SkillOwner, TargetingProperties);

		if (Effect->SkillCollisionType != ESkillCollisionType::Single)
		{
			TArray<AAutobattlerCharacter*> SecondaryTargets;
			if (Effect->SkillCollisionType == ESkillCollisionType::Box)
			{
				UAutobattlerFunctionLibrary::GetCharactersFilteredInBox(
					SecondaryTargets,
					SkillOwner,
					CharacterCollisionChannel,
					GetActorLocation(),
					Effect->SkillBoxExtent,
					Effect->SecondaryTargetFilter
				);
			}
			else if (Effect->SkillCollisionType == ESkillCollisionType::Sphere)
			{
				UAutobattlerFunctionLibrary::GetCharactersFilteredInSphere(
					SecondaryTargets,
					SkillOwner,
					CharacterCollisionChannel,
					GetActorLocation(),
					Effect->SkillCollisionRadius,
					Effect->SecondaryTargetFilter
				);
			}

			for (auto SecondaryTarget : SecondaryTargets)
			{
				if (SecondaryTarget == TargetingProperties.TargetCharacter) continue;
				if (ShouldExecuteSkillList) Effect->ExecuteSkill(
					SkillOwner,
					TargetingProperties.TargetingMode,
					SecondaryTarget,
					TargetingProperties.TargetLocation
				);

				if (SkillImplementation->HasDuration) UDurationSkillComponent::ConfigureTimedSkill(SecondaryTarget, SkillImplementation, SkillOwner, TargetingProperties);
			}
		}
	}
}

void AExecuteSkill::UpdateEffectVisuals_Implementation(UStaticMesh* NewMesh, UParticleSystem* NewTemplate)
{
	EffectMesh->SetStaticMesh(NewMesh);
	EffectParticle->SetTemplate(NewTemplate);	
}

void AExecuteSkill::OnEffectEnd()
{
	Destroy();
}

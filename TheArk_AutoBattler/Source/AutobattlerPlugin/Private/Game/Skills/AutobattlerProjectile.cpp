// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "Game/Skills/AutobattlerProjectile.h"

/* Autobattler includes. */
#include "DataAssets/AutobattlerConfiguration.h"
#include "DataAssets/AutobattlerSkill.h"
#include "DataAssets/DefaultAttack.h"
#include "Game/Skills/ExecuteSkill.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AAutobattlerProjectile::AAutobattlerProjectile()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticle"));
	ProjectileParticle->SetupAttachment(RootComponent);

 	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAutobattlerProjectile::InitialiseProjectile(const UAutobattlerSkill* SkillImplementation, AAutobattlerCharacter* SkillOwner, const FAbilityTargetingProperties& TargetingProperties)
{
	if (TargetingProperties.TargetingMode != ESkillTargetingMode::Actor)
	{
		UAutobattlerFunctionLibrary::PrintWarningToLog(FString::Printf(TEXT("%s : [InitialiseProjectile] Non-actor targeting modes are not supported for projectiles"), *GetName()));
		return;
	}

	if (!IsValid(TargetingProperties.TargetCharacter))
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [InitialiseProjectile] Passed invalid target character!"), *GetName()));
		return;
	}

	TargetingProperties.TargetCharacter->ActionChanged.AddDynamic(this, &AAutobattlerProjectile::OnTargetCharacterStateChange);

	const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this);
	MinDistanceToTarget = Configuration != nullptr ? Configuration->ProjectileMinDistanceToHit : 32.0f;

	UpdateVisuals(SkillImplementation->SkillMesh, SkillImplementation->SkillParticleEffect);
	SkillOwnerInternal = SkillOwner;
	SkillImplementationInternal = SkillImplementation;
	ProjectileTargetingProperties = TargetingProperties;
	UpdateProjectileTarget(TargetingProperties.TargetCharacter->GetRootComponent());
	ShouldCheckDestination = true;
}

void AAutobattlerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority() && ShouldCheckDestination)
	{
		if (ProjectileTargetingProperties.TargetingMode == ESkillTargetingMode::Actor && IsValid(ProjectileTargetingProperties.TargetCharacter))
		{
			float CurrentDistance = UAutobattlerFunctionLibrary::CartesianDistance(GetActorLocation(), ProjectileTargetingProperties.TargetCharacter->GetActorLocation());
			if (CurrentDistance <= MinDistanceToTarget)
			{
				OnArriveAtDestination();
				ShouldCheckDestination = false;
			}
		}
		else
		{
			ShouldCheckDestination = false;
			Destroy();
		}
	}

	if (IsValid(CurrentProjectileHomingTarget))
	{
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), CurrentProjectileHomingTarget->GetComponentLocation(), DeltaTime, 500.0f)); // TODO: 2.0f should be configurable.
	}
}

void AAutobattlerProjectile::UpdateVisuals_Implementation(UStaticMesh* NewMesh, UParticleSystem* NewParticleSystem)
{
	ProjectileMesh->SetStaticMesh(NewMesh);
	ProjectileParticle->SetTemplate(NewParticleSystem);	
}

void AAutobattlerProjectile::UpdateProjectileTarget_Implementation(USceneComponent* ProjectileHomingTarget)
{
	CurrentProjectileHomingTarget = ProjectileHomingTarget;
}

void AAutobattlerProjectile::OnArriveAtDestination()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (SkillImplementationInternal != nullptr)
	{
		AExecuteSkill* ExecuteSkill = GetWorld()->SpawnActor<AExecuteSkill>(
			SkillImplementationInternal->ExecuteSkillClass,
			GetActorTransform(),
			ActorSpawnParams
		);

		ExecuteSkill->InitialiseAsInstant(SkillImplementationInternal, SkillOwnerInternal, ProjectileTargetingProperties);
	}
	
	Destroy();
}

void AAutobattlerProjectile::OnTargetCharacterStateChange(EActionType NewAction, AAutobattlerCharacter* UpdatedCharacter)
{
	if (NewAction == EActionType::Dead)
	{
		Destroy();
	}
}

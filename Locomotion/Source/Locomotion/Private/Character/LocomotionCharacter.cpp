// Copyright Robert Zygmunt Uszynski 2019-2020

#include "..\..\Public\Character\LocomotionCharacter.h"
#include "..\..\Public\Character\LocomotionAnimInstance.h"
#include "..\..\Public\Character\LocomotionAnimInstancePP.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ALocomotionCharacter::ALocomotionCharacter()
{
	WallClimbTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WallClimbComponent"));
	OnWallClimbingTimelineCallback.BindUFunction(this, FName("WallClimbTimelineCallback"));

	PrimaryActorTick.bCanEverTick = true;

	bShouldSprint = false;
	ForwardAxisValue = 0.0f;
	RightAxisValue = 0.0f;

	WalkingSpeed = 150.0f;
	RunningSpeed = 375.0f;
	SprintingSpeed = 600.0f;
	CrouchingSpeed = 150.0f;
	WalkingAcceleration = 800.0f;
	RunningAcceleration = 1000.0f;
	WalkingDeceleration = 800.0f;
	RunningDeceleration = 800.0f;
	WalkingFriction = 8.0f;
	RunningFriction = 6.0f;

	MovementInput = FVector(0.0f, 0.0f, 0.0f);
	CharacterRotation = FRotator(0.0f, 0.0f, 0.0f);
	LookingRotation = FRotator(0.0f, 0.0f, 0.0f);
	LastVelocityRotation = FRotator(0.0f, 0.0f, 0.0f);
	LastMovementInputRotation = FRotator(0.0f, 0.0f, 0.0f);
	AimYawDelta = 0.0f;
	AimYawRate = 0.0f;
	MovementInputVelocityDifference = 0.0f;
	TargetCharacterRotationDifference = 0.0f;
	Direction = 0.0f;
	bIsMoving = false;
	bHasMovementInput = false;

	Gait = EGait::Running;
	Stance = EStance::Standing;
	RotationMode = ERotationMode::VelocityDirection;
	MovementType = EMovementType::Grounded;
	MovementTypePrev = EMovementType::None;
	bIsAiming = false;

	AllowedGaits.Add(EGait::Walking);
	AllowedGaits.Add(EGait::Running);
	AllowedGaits.Add(EGait::Sprinting);

	AllowedStances.Add(EStance::Crouching);
	AllowedStances.Add(EStance::Standing);

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	WallClimbCollisionChannel = TEnumAsByte<ECollisionChannel>(ECollisionChannel::ECC_Visibility);

	GroundedWallClimbTraceSettings.MinLedgeHeight = 50.0f;
	GroundedWallClimbTraceSettings.MaxLedgeHeight = 250.0f;
	GroundedWallClimbTraceSettings.ReachDistance = 75.0f;
	GroundedWallClimbTraceSettings.ForwardTraceRadius = 30.0f;
	GroundedWallClimbTraceSettings.DownTraceRadius = 30.0f;

	FallingWallClimbTraceSettings.MinLedgeHeight = 50.0f;
	FallingWallClimbTraceSettings.MaxLedgeHeight = 150.0f;
	FallingWallClimbTraceSettings.ReachDistance = 70.0f;
	FallingWallClimbTraceSettings.ForwardTraceRadius = 30.0f;
	FallingWallClimbTraceSettings.DownTraceRadius = 30.0f;

	PelvisBone = FName("pelvis");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);

	bRagdollOnGround = false;
	RagdollLocation = FVector(0.0f, 0.0f, 0.0f);
	RagdollVelocity = FVector(0.0f, 0.0f, 0.0f);

	TraceActorsToIgnore.Init(this, 1);
	bGateOneOpen = true;
	bGateTwoOpen = true;
}

void ALocomotionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateEssentialVariables();
	SprintCheck();
	ManageCharacterRotation();
	switch (MovementType)
	{
	case EMovementType::Grounded:
		if (Stance == EStance::Standing && (Gait == EGait::Running || Gait == EGait::Sprinting))
		{
			CustomAcceleration();
		}
		break;

	case EMovementType::Ragdoll:
	{
		GetMesh()->SetAllMotorsAngularDriveParams(UKismetMathLibrary::MapRangeClamped(ChooseVelocity().Size(), 0.0f, 1000.0f, 0.0, 25000.0f), 0.0f, 0.0f, false);
		if (ChooseVelocity().Z < -4000.0f) {
			GetMesh()->SetEnableGravity(false);
		}
		else
		{
			GetMesh()->SetEnableGravity(true);
		}

		RagdollVelocity = ChooseVelocity();
		RagdollLocation = GetMesh()->GetSocketLocation(PelvisBone);
		FRotator LocalActorRotation;
		FVector LocalActorLocation;
		CalculateActorLocationAndRotationWhileRagdolling(GetMesh()->GetSocketRotation(PelvisBone), RagdollLocation, LocalActorRotation, LocalActorLocation);
		SetActorLocation(LocalActorLocation);
		TargetRotation = LocalActorRotation;
		TargetCharacterRotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation).Yaw;
		CharacterRotation = LocalActorRotation;
		SetActorRotation(CharacterRotation);
		break;
	}
	}
}

void ALocomotionCharacter::SetRotationMode(ERotationMode NewRotationMode)
{
	if (NewRotationMode != RotationMode)
	{
		RotationMode = NewRotationMode;
		ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
		if (LAI != nullptr) LAI->SetRotationMode(RotationMode);
		if (bIsMoving) RotationMultiplier = 0.0f;
	}
}

void ALocomotionCharacter::SetIsAiming(bool NewAiming)
{
	if (NewAiming != bIsAiming)
	{
		bIsAiming = NewAiming;
		UpdateCharacterMovement();
		ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
		if (LAI != nullptr) LAI->SetAiming(bIsAiming);
	}
}

void ALocomotionCharacter::SetGait(EGait NewGait)
{
	if (NewGait != Gait && AllowedGaits.Contains(NewGait))
	{
		Gait = NewGait;
		UpdateCharacterMovement();
		ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
		if (LAI != nullptr) LAI->SetGait(Gait);
	}
}

void ALocomotionCharacter::SetMovementType(EMovementType NewMovementType)
{
	if (NewMovementType != MovementType)
	{
		MovementTypePrev = MovementType;
		MovementType = NewMovementType;
		UpdateCharacterMovement();

		ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
		if (LAI != nullptr) LAI->SetMovementType(MovementType);

		ULocomotionAnimInstancePP* LAIPP = Cast<ULocomotionAnimInstancePP>(GetMesh()->GetPostProcessInstance());
		if (LAIPP != nullptr) LAIPP->SetMovementType(MovementType);

		switch (MovementTypePrev)
		{
		case EMovementType::Grounded:
			RotationOffset = 0.0f;
			if (bIsMoving)
			{
				JumpRotation = LastVelocityRotation;
			}
			else
			{
				JumpRotation = CharacterRotation;
			}
			break;
			
		case EMovementType::Ragdoll:
			JumpRotation = CharacterRotation;
			break;
		}
	}
}

void ALocomotionCharacter::SetStance(EStance NewStance)
{
	Stance = NewStance;
	UpdateCharacterMovement();

	ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
	if (LAI != nullptr) LAI->SetStance(Stance);

	ULocomotionAnimInstancePP* LAIPP = Cast<ULocomotionAnimInstancePP>(GetMesh()->GetPostProcessInstance());
	if (LAIPP != nullptr) LAIPP->SetStance(Stance);
}

void ALocomotionCharacter::SetUpperBodyLayeringIndex(int32 NewUpperBodyLayeringIndex)
{
	int32 LocalIndex = 0;
	if (NewUpperBodyLayeringIndex > 0) LocalIndex = NewUpperBodyLayeringIndex;
	UpperBodyLayeringIndex = LocalIndex;

	ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
	if (LAI != nullptr) LAI->SetUpperBodyLayeringIndex(UpperBodyLayeringIndex);
}

void ALocomotionCharacter::SetFullBodyLayeringIndex(int32 NewFullBodyLayeringIndex)
{
	int32 LocalIndex = 0;
	if (NewFullBodyLayeringIndex > 0) LocalIndex = NewFullBodyLayeringIndex;
	FullBodyLayeringIndex = LocalIndex;

	ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
	if (LAI != nullptr) LAI->SetFullBodyLayeringIndex(FullBodyLayeringIndex);
}

void ALocomotionCharacter::SetAllowedMovementModels(const TArray<EGait> NewAllowedGaits, const TArray<EStance> NewAllowedStances)
{
	if (NewAllowedGaits.Num() > 0)
	{
		AllowedGaits = NewAllowedGaits;
		if (!AllowedGaits.Contains(Gait))
		{
			SetGait(AllowedGaits[0]);
		}
	}
	
	if (NewAllowedStances.Num() > 0)
	{
		AllowedStances = NewAllowedStances;
		if (!AllowedStances.Contains(Stance))
		{
			StanceAction();
		}
	}
}

void ALocomotionCharacter::SetProp(UStaticMeshComponent *& NewlySpawnedStaticMeshComp, USkeletalMeshComponent *& NewlySpawnedSkeletalMeshComp, FName SocketName, bool ClearAll, bool Clear, UStaticMesh * StaticMesh, USkeletalMesh * SkeletalMesh)
{
	if (ClearAll)
	{
		if (SpawnedStaticMeshComps.Num() > 0)
		{
			for (auto SpawnedStaticMeshComp : SpawnedStaticMeshComps)
			{
				SpawnedStaticMeshComps.Remove(SpawnedStaticMeshComp);
				SpawnedStaticMeshComp->DestroyComponent();
			}
		}

		if (SpawnedSkeletalMeshComps.Num() > 0)
		{
			for (auto SpawnedSkeletalMeshComp : SpawnedSkeletalMeshComps)
			{
				SpawnedSkeletalMeshComps.Remove(SpawnedSkeletalMeshComp);
				SpawnedSkeletalMeshComp->DestroyComponent();
			}
		}
	}
	else if (Clear)
	{
		if (SpawnedStaticMeshComps.Num() > 0 && bWasLastSpawnedCompStaticMesh)
		{
			UStaticMeshComponent* LastStaticMeshComp = SpawnedStaticMeshComps[SpawnedStaticMeshComps.Num() - 1];
			SpawnedStaticMeshComps.Remove(LastStaticMeshComp);
			LastStaticMeshComp->DestroyComponent();
		}
		else if (SpawnedSkeletalMeshComps.Num() > 0 && !bWasLastSpawnedCompStaticMesh)
		{
			USkeletalMeshComponent* LastSkeletalMeshComp = SpawnedSkeletalMeshComps[SpawnedSkeletalMeshComps.Num() - 1];
			SpawnedSkeletalMeshComps.Remove(LastSkeletalMeshComp);
			LastSkeletalMeshComp->DestroyComponent();
		}
	}

	if (!GetMesh()->DoesSocketExist(SocketName)) return;

	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,true);

	if (SkeletalMesh != nullptr)
	{
		FString NewCompString("AttachedSkeletalMeshComp_");
		NewCompString.AppendInt(SpawnedSkeletalMeshComps.Num() + 1);
		FName NewCompName = FName(NewCompString);
		USkeletalMeshComponent* NewSkeletalMeshComp = NewObject<USkeletalMeshComponent>(this, NewCompName);
		if (NewSkeletalMeshComp != nullptr)
		{
			NewSkeletalMeshComp->RegisterComponent();
			NewSkeletalMeshComp->AttachToComponent(GetMesh(), AttachmentTransformRules, SocketName);
			NewSkeletalMeshComp->SetSkeletalMesh(SkeletalMesh);
			NewlySpawnedSkeletalMeshComp = NewSkeletalMeshComp;
			SpawnedSkeletalMeshComps.Add(NewSkeletalMeshComp);
		}
	}
	else if (StaticMesh != nullptr)
	{
		FString NewCompString("AttachedStaticMeshComp_");
		NewCompString.AppendInt(SpawnedSkeletalMeshComps.Num() + 1);
		FName NewCompName = FName(NewCompString);
		UStaticMeshComponent* NewStaticMeshComp = NewObject<UStaticMeshComponent>(this, NewCompName);
		if (NewStaticMeshComp != nullptr)
		{
			NewStaticMeshComp->RegisterComponent();
			NewStaticMeshComp->AttachToComponent(GetMesh(), AttachmentTransformRules, SocketName);
			NewStaticMeshComp->SetStaticMesh(StaticMesh);
			NewlySpawnedStaticMeshComp = NewStaticMeshComp;
			SpawnedStaticMeshComps.Add(NewStaticMeshComp);
		}
	}
}

void ALocomotionCharacter::GetMovementProperties(FVector & CurrentVelocity, FRotator & CurrentCharacterRotation, FRotator & CurrentLastVelocityRotation, FRotator & CurrentLastMovementInputRotation, FRotator & CurrentLookingRotation, float & CurrentDirection, float & CurrentTargetCharacterRotationDifference, float & CurrentMovementInputVelocityDifference, float & CurrentAimYawDelta, float & CurrentAimYawRate, bool & bIsCurrentMoving, bool & bCurrentHasMovementInput)
{
	CurrentVelocity = GetVelocity();
	CurrentCharacterRotation = CharacterRotation;
	CurrentLastVelocityRotation = LastVelocityRotation;
	CurrentLastMovementInputRotation = LastMovementInputRotation;
	CurrentLookingRotation = LookingRotation;
	CurrentDirection = Direction;
	CurrentTargetCharacterRotationDifference = TargetCharacterRotationDifference;
	CurrentMovementInputVelocityDifference = MovementInputVelocityDifference;
	CurrentAimYawDelta = AimYawDelta;
	CurrentAimYawRate = AimYawRate;
	bIsCurrentMoving = bIsMoving;
	bCurrentHasMovementInput = bHasMovementInput;
}

void ALocomotionCharacter::GetMovementStates(ERotationMode & CurrentRotationMode, EGait & CurrentGait, EMovementType & CurrentMovementType, EMovementType & PreviousMovementType, EStance & CurrentStance, bool & bIsCurrentlyAiming)
{
	CurrentRotationMode = RotationMode;
	CurrentGait = Gait;
	CurrentMovementType = MovementType;
	PreviousMovementType = MovementTypePrev;
	CurrentStance = Stance;
	bIsCurrentlyAiming = bIsAiming;
}

void ALocomotionCharacter::GetLayeringIndicies(int32 & CurrentUpperBodyLayeringIndex, int32 & CurrentFullBodyLayeringIndex)
{
	CurrentUpperBodyLayeringIndex = UpperBodyLayeringIndex;
	CurrentFullBodyLayeringIndex = FullBodyLayeringIndex;
}

void ALocomotionCharacter::WantsToClimb()
{
	if (bHasMovementInput)
	{
		switch (MovementType)
		{
		case EMovementType::Grounded:
			WallClimbCheck(GroundedWallClimbTraceSettings);
			break;
		case EMovementType::Falling:
			WallClimbCheck(FallingWallClimbTraceSettings);
			break;
		}
	}
}

FVector ALocomotionCharacter::ChooseVelocity()
{
	switch (MovementType)
	{
	case EMovementType::Ragdoll:
		return GetMesh()->GetPhysicsLinearVelocity(PelvisBone);

	default:
		return GetVelocity();
	}

	return GetVelocity();
}

void ALocomotionCharacter::AddCharacterRotation(FRotator AddAmount)
{
	TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, UKismetMathLibrary::NegateRotator(AddAmount));
	TargetCharacterRotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation).Yaw;
	CharacterRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, UKismetMathLibrary::NegateRotator(AddAmount));
	SetActorRotation(CharacterRotation);
}

FRotator ALocomotionCharacter::SetLookingRotation_Implementation()
{
	return GetControlRotation();
}

void ALocomotionCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->AddTickPrerequisiteActor(this);

	if (WallClimbTimelineCurveLow)
	{
		WallClimbTimeline->AddInterpFloat(WallClimbTimelineCurveLow, OnWallClimbingTimelineCallback, FName("WallClimbTrack"), FName("WallClimbTrack"));
	}

	FOnTimelineEventStatic OnWallClimbingTimelineFinishedCallback;
	OnWallClimbingTimelineFinishedCallback.BindUFunction(this, FName("WallClimbEnd"));
	WallClimbTimeline->SetTimelineFinishedFunc(OnWallClimbingTimelineFinishedCallback);
	UpdateCharacterMovement();
}

void ALocomotionCharacter::OnConstruction(const FTransform & Transform)
{
	//if (AllowedGaits.Contains(DefaultGait))
//	{
	//	Gait = DefaultGait;
	//	UpdateCharacterMovement();
		//ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
		//if (LAI != nullptr) LAI->SetGait(DefaultGait);
	//}
	// SetStance(DefaultStance);
}

void ALocomotionCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
}

void ALocomotionCharacter::MoveForward(float Value)
{
	ForwardAxisValue = Value;
	if (Value != 0.0f)
	{
		FRotator ControlRotation = GetControlRotation();
		FRotator ControlRotationYaw = FRotator(0.0f, ControlRotation.Yaw, 0.0f);
		FVector MovementDirection = UKismetMathLibrary::GetForwardVector(ControlRotationYaw);
		AddMovementInput(MovementDirection, Value, false);
	}
}

void ALocomotionCharacter::MoveRight(float Value)
{
	RightAxisValue = Value;
	if (Value != 0.0f)
	{
		FRotator ControlRotation = GetControlRotation();
		FRotator ControlRotationYaw = FRotator(0.0f, ControlRotation.Yaw, 0.0f);
		FVector MovementDirection = UKismetMathLibrary::GetRightVector(ControlRotationYaw);
		AddMovementInput(MovementDirection, Value, false);
	}
}

void ALocomotionCharacter::JumpActionPressed()
{
	if (!IsPlayingRootMotion())
	{
		switch (Stance)
		{
		case EStance::Standing:
			Jump();
			break;

		case EStance::Crouching:
			UnCrouch();
			SetStance(EStance::Standing);
			OnStanceChange(EStance::Standing);
			break;
		}	
	}
}

void ALocomotionCharacter::JumpActionReleased()
{
	StopJumping();
}

void ALocomotionCharacter::StanceAction()
{
	if (MovementType == EMovementType::Grounded)
	{
		switch (Stance)
		{
		case EStance::Standing:
			if (AllowedStances.Contains(EStance::Crouching))
			{
				Crouch();
				OnStanceChange(EStance::Crouching);
			}
			break;

		case EStance::Crouching:
			if (AllowedStances.Contains(EStance::Standing))
			{
				UnCrouch();
				OnStanceChange(EStance::Standing);
			}
			break;
		}
	}
}

void ALocomotionCharacter::OnStanceChange(EStance EnteredStance)
{
	/* Function exists to be overwritten. */
}

void ALocomotionCharacter::WalkAction()
{
	switch (Gait)
	{
	case EGait::Walking:
		SetGait(EGait::Running);
		break;

	case EGait::Running:
		SetGait(EGait::Walking);
		break;
	}
}

void ALocomotionCharacter::SprintActionPressed()
{
	bShouldSprint = true;
	if (Gait == EGait::Walking)
	{
		SetGait(EGait::Running);
	}
}

void ALocomotionCharacter::SprintActionReleased()
{
	bShouldSprint = false;
}

void ALocomotionCharacter::AimActionStart()
{
	if (RotationMode == ERotationMode::VelocityDirection)
	{
		SetRotationMode(ERotationMode::TargetDirection);
	}

	SetIsAiming(true);
}

void ALocomotionCharacter::AimActionStop()
{
	SetIsAiming(false);
}

void ALocomotionCharacter::NewRotationMode()
{
	switch (RotationMode)
	{
	case ERotationMode::VelocityDirection:
		SetRotationMode(ERotationMode::TargetDirection);
		break;

	case ERotationMode::TargetDirection:
		SetRotationMode(ERotationMode::VelocityDirection);
		break;
	}
}

void ALocomotionCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PrevCustomMode);
	if (GetCharacterMovement()->MovementMode.GetValue() == EMovementMode::MOVE_Walking ||
		GetCharacterMovement()->MovementMode.GetValue() == EMovementMode::MOVE_NavWalking)
	{
		SetMovementType(EMovementType::Grounded);
	}
	else if (GetCharacterMovement()->MovementMode.GetValue() == EMovementMode::MOVE_Falling)
	{
		SetMovementType(EMovementType::Falling);
	}
}

void ALocomotionCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetStance(EStance::Crouching);
}

void ALocomotionCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetStance(EStance::Standing);
}

void ALocomotionCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);
	if (bHasMovementInput)
	{
		GetCharacterMovement()->BrakingFrictionFactor = 0.5f;
		GetWorld()->GetTimerManager().SetTimer(LandedTimerHandle, this, &ALocomotionCharacter::OnLandedTimerHandleEnd, 0.2f, false);
	}
	else
	{
		GetCharacterMovement()->BrakingFrictionFactor = 3.0f;
		GetWorld()->GetTimerManager().SetTimer(LandedTimerHandle, this, &ALocomotionCharacter::OnLandedTimerHandleEnd, 0.2f, false);
	}
}

void ALocomotionCharacter::ToRagdoll(FVector HitDirection)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetMovementType(EMovementType::Ragdoll);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, true, true);
	GetMesh()->SetSimulatePhysics(true);
	if (GetMesh()->DoesSocketExist(ImpulseBoneName))
	{
		GetMesh()->AddImpulse(HitDirection * ImpulseModifier, ImpulseBoneName, true);
	}
}

void ALocomotionCharacter::UpdateCharacterMovement()
{
	float ChosenWalkSpeed = ChooseMaxWalkSpeed();
	GetCharacterMovement()->MaxWalkSpeed = ChosenWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = ChosenWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = ChooseMaxAcceleration();
	GetCharacterMovement()->BrakingDecelerationWalking = ChooseDeceleration();
	GetCharacterMovement()->GroundFriction = ChooseGroundFriction();
}

float ALocomotionCharacter::ChooseMaxWalkSpeed()
{
	switch (Stance)
	{
	case EStance::Standing:
		if (bIsAiming)
		{
			switch (Gait)
			{
			case EGait::Walking:
				return WalkingSpeed;
			case EGait::Running:
				return WalkingSpeed;
			case EGait::Sprinting:
				return RunningSpeed;
			}
		}
		else
		{
			switch (Gait)
			{
			case EGait::Walking:
				return WalkingSpeed;
			case EGait::Running:
				return RunningSpeed;
			case EGait::Sprinting:
				return SprintingSpeed;
			}
		}
		break;

	case EStance::Crouching:
		return CrouchingSpeed;
	}
	return 0.0f;
}

float ALocomotionCharacter::ChooseGroundFriction()
{
	switch (Gait)
	{
	case EGait::Walking:
		return WalkingFriction;
	case EGait::Running:
		return RunningFriction;
	case EGait::Sprinting:
		return RunningFriction;
	}
	return 0.0f;
}

float ALocomotionCharacter::ChooseMaxAcceleration()
{
	switch (Gait)
	{
	case EGait::Walking:
		return WalkingAcceleration;
	case EGait::Running:
		return RunningAcceleration;
	case EGait::Sprinting:
		return RunningAcceleration;
	}
	return 0.0f;
}

float ALocomotionCharacter::ChooseDeceleration()
{
	switch (Gait)
	{
	case EGait::Walking:
		return WalkingDeceleration;
	case EGait::Running:
		return RunningDeceleration;
	case EGait::Sprinting:
		return RunningDeceleration;
	}
	return 0.0f;
}

FVector ALocomotionCharacter::GetCapsuleFloorLocation(float ZOffset)
{
	UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();
	return CharacterCapsule->GetComponentLocation() - (CharacterCapsule->GetUpVector() * (CharacterCapsule->GetScaledCapsuleHalfHeight() + ZOffset));
}

FTransform ALocomotionCharacter::SubTransformByComponents(FTransform A, FTransform B)
{
	FRotator ARotation = FRotator(A.GetRotation());
	FRotator BRotation = FRotator(B.GetRotation());
	FRotator FinalRot = FRotator(ARotation.Pitch - BRotation.Pitch, ARotation.Yaw - BRotation.Yaw, ARotation.Roll - BRotation.Roll);

	return FTransform(FQuat(FinalRot), A.GetLocation() - B.GetLocation(), A.GetScale3D() - B.GetScale3D());
}

FTransform ALocomotionCharacter::AddTransformByComponents(FTransform A, FTransform B)
{
	FRotator ARotation = FRotator(A.GetRotation());
	FRotator BRotation = FRotator(B.GetRotation());
	FRotator FinalRot = FRotator(ARotation.Pitch + BRotation.Pitch, ARotation.Yaw + BRotation.Yaw, ARotation.Roll + BRotation.Roll);

	return FTransform(FQuat(FinalRot), A.GetLocation() + B.GetLocation(), A.GetScale3D() + B.GetScale3D());
}

bool ALocomotionCharacter::WallClimbCheck(FWallClimbTraceSettings WallClimbTraceSettings)
{
	FVector ControlForwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
	FVector ControlRightVector = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
	FVector PlayerInputVector = (ControlForwardVector * ForwardAxisValue) + (ControlRightVector * RightAxisValue);
	UKismetMathLibrary::Vector_Normalize(PlayerInputVector, 0.0001f);

	FVector StartTrace = (GetCapsuleFloorLocation(2.0f) + (PlayerInputVector * -30.0f)) + FVector(0.0f, 0.0f, (WallClimbTraceSettings.MaxLedgeHeight + WallClimbTraceSettings.MinLedgeHeight) / 2.0f);
	FVector EndTrace = StartTrace + (PlayerInputVector * WallClimbTraceSettings.ReachDistance);

	float TraceHalfHeight = ((WallClimbTraceSettings.MaxLedgeHeight - WallClimbTraceSettings.MinLedgeHeight) / 2.0f) + 1.0f;

	FHitResult InitialHit;
	FVector InitialTraceImpact;
	FVector InitialTraceNormal;
	bool bInitialHit = UKismetSystemLibrary::CapsuleTraceSingle(this, StartTrace, EndTrace, WallClimbTraceSettings.ForwardTraceRadius, TraceHalfHeight, UEngineTypes::ConvertToTraceType(WallClimbCollisionChannel.GetValue()), false, TraceActorsToIgnore, EDrawDebugTrace::Type::None, InitialHit, true);
	if (InitialHit.bBlockingHit && !InitialHit.bStartPenetrating && !GetCharacterMovement()->IsWalkable(InitialHit))
	{
		InitialTraceImpact = FVector(InitialHit.ImpactPoint.X, InitialHit.ImpactPoint.Y, InitialHit.ImpactPoint.Z);
		InitialTraceNormal = FVector(InitialHit.ImpactNormal.X, InitialHit.ImpactNormal.Y, InitialHit.ImpactNormal.Z);
	}
	else
	{
		return false;
	}

	EndTrace = FVector(InitialTraceImpact.X, InitialTraceImpact.Y, GetCapsuleFloorLocation(2.0f).Z) + (InitialTraceNormal * -15.0f);
	StartTrace = EndTrace + FVector(0.0f, 0.0f, WallClimbTraceSettings.MaxLedgeHeight + WallClimbTraceSettings.DownTraceRadius + 1.0f);

	FHitResult WalkableHit;
	FVector DownTraceLocation;
	UPrimitiveComponent* HitComponent;
	bool bWalkableHit = UKismetSystemLibrary::SphereTraceSingle(this, StartTrace, EndTrace, WallClimbTraceSettings.DownTraceRadius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), false, TraceActorsToIgnore, EDrawDebugTrace::Type::None, WalkableHit, true);
	if (WalkableHit.bBlockingHit && GetCharacterMovement()->IsWalkable(WalkableHit))
	{
		DownTraceLocation = FVector(WalkableHit.Location.X, WalkableHit.Location.Y, WalkableHit.ImpactPoint.Z);
		HitComponent = WalkableHit.GetComponent();
	}
	else
	{
		return false;
	}

	FVector AdjustedCapsuleLocation = DownTraceLocation + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0f);
	StartTrace = AdjustedCapsuleLocation + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere());
	EndTrace = AdjustedCapsuleLocation - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere());

	FHitResult RoomHit;
	FTransform TargetTransform;
	float WallClimbHeight;
	bool bRoomHit = UKismetSystemLibrary::SphereTraceSingleByProfile(this, StartTrace, EndTrace, GetCapsuleComponent()->GetScaledCapsuleRadius(), FName("Pawn"), false, TraceActorsToIgnore, EDrawDebugTrace::Type::None, RoomHit, true);
	if (!(RoomHit.bBlockingHit || RoomHit.bStartPenetrating))
	{
		TargetTransform = FTransform(FQuat(UKismetMathLibrary::Conv_VectorToRotator(InitialTraceNormal * FVector(-1.0f, -1.0f, 0.0f))), AdjustedCapsuleLocation);
		WallClimbHeight = AdjustedCapsuleLocation.Z - GetActorLocation().Z;
	}
	else
	{
		return false;
	}

	EWallClimbType WallClimbType = EWallClimbType::Low;
	switch (MovementType)
	{
	case EMovementType::Falling:
		WallClimbType = EWallClimbType::Fall;
		break;

	default:
		WallClimbType = WallClimbHeight > 125.0f ? EWallClimbType::High : EWallClimbType::Low;
		break;
	}
	
	FClimbableLedgeData ClimbableLedgeData;
	ClimbableLedgeData.TargetTransform = TargetTransform;
	ClimbableLedgeData.HitComponent = HitComponent;
	WallClimbBegin(WallClimbHeight, ClimbableLedgeData, WallClimbType);
	return true;
}

void ALocomotionCharacter::WallClimbBegin(float WallClimbHeight, FClimbableLedgeData ClimbableLedgeData, EWallClimbType WallClimbType)
{
	ULocomotionAnimInstance* LAI = Cast<ULocomotionAnimInstance>(GetMesh()->GetAnimInstance());
	if (LAI == nullptr) return;
	FWallClimbAssetData LocalAssetData = LAI->GetWallClimbDataFromType(WallClimbType);

	LocalWallClimbParameters.AnimMontage = LocalAssetData.AnimMontage;
	LocalWallClimbParameters.VectorAdjustCurve = LocalAssetData.VectorAdjustCurve;
	LocalWallClimbParameters.StartingOffset = LocalAssetData.StartingOffset;
	LocalWallClimbParameters.PlayRate = UKismetMathLibrary::MapRangeClamped(WallClimbHeight, LocalAssetData.LowHeight, LocalAssetData.HighHeight, LocalAssetData.LowPlayRate, LocalAssetData.HighPlayRate);
	LocalWallClimbParameters.StartPosition = UKismetMathLibrary::MapRangeClamped(WallClimbHeight, LocalAssetData.LowHeight, LocalAssetData.HighHeight, LocalAssetData.LowStartPosition, LocalAssetData.HighStartPosition);

	WallTargetTransform = ClimbableLedgeData.TargetTransform;
	WallClimbActualStartOffset = SubTransformByComponents(GetActorTransform(), WallTargetTransform);

	FVector AnimAdjustedPosition = UKismetMathLibrary::Conv_RotatorToVector(FRotator(WallTargetTransform.GetRotation())) * LocalWallClimbParameters.StartingOffset.Y;
	FTransform AnimTransform = FTransform(WallTargetTransform.GetRotation(), WallTargetTransform.GetLocation() - FVector(AnimAdjustedPosition.X, AnimAdjustedPosition.Y, LocalWallClimbParameters.StartingOffset.Z));
	WallClimbAnimStartOffset = SubTransformByComponents(AnimTransform, WallTargetTransform);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetMovementType(EMovementType::WallWaking);

	float TimelineMin = 0.0f;
	float TimelineMax = 0.0f;
	LocalWallClimbParameters.VectorAdjustCurve->GetTimeRange(TimelineMin, TimelineMax);

	switch (WallClimbType)
	{
	case EWallClimbType::Low:
		if (WallClimbTimelineCurveLow)
		{
			WallClimbTimeline->SetFloatCurve(WallClimbTimelineCurveLow, FName("WallClimbTrack"));
		}
		break;

	default:
		if (WallClimbTimelineCurveHigh)
		{
			WallClimbTimeline->SetFloatCurve(WallClimbTimelineCurveHigh, FName("WallClimbTrack"));
		}
		break;
	}

	WallClimbTimeline->SetTimelineLength(TimelineMax - LocalWallClimbParameters.StartPosition);
	WallClimbTimeline->SetPlayRate(LocalWallClimbParameters.PlayRate);
	WallClimbTimeline->PlayFromStart();

	if (LocalWallClimbParameters.AnimMontage)
	{
		LAI->Montage_Play(LocalWallClimbParameters.AnimMontage, LocalWallClimbParameters.PlayRate, EMontagePlayReturnType::MontageLength, LocalWallClimbParameters.StartPosition, false);
	}
}

void ALocomotionCharacter::WallClimbUpdate(float Blend)
{
	FVector CurrentVectorCurveValue = LocalWallClimbParameters.VectorAdjustCurve->GetVectorValue(LocalWallClimbParameters.StartPosition + WallClimbTimeline->GetPlaybackPosition());
	float PositionAlpha = CurrentVectorCurveValue.X;
	float XYCorrectionAlpha = CurrentVectorCurveValue.Y;
	float ZCorrectionAlpha = CurrentVectorCurveValue.Z;

	FVector XYCorrectedLoc = FVector(WallClimbAnimStartOffset.GetLocation().X, WallClimbAnimStartOffset.GetLocation().Y, WallClimbActualStartOffset.GetLocation().Z);
	FVector ZCorrectedLoc = FVector(WallClimbActualStartOffset.GetLocation().X, WallClimbActualStartOffset.GetLocation().Y, WallClimbAnimStartOffset.GetLocation().Z);

	FTransform XYCorrected = UKismetMathLibrary::TLerp(WallClimbActualStartOffset, FTransform(WallClimbAnimStartOffset.GetRotation(), XYCorrectedLoc), XYCorrectionAlpha);
	FTransform ZCorrected = UKismetMathLibrary::TLerp(WallClimbActualStartOffset, FTransform(WallClimbActualStartOffset.GetRotation(), ZCorrectedLoc), ZCorrectionAlpha);

	FTransform XYZCorrected = FTransform(XYCorrected.GetRotation(), FVector(XYCorrected.GetLocation().X, XYCorrected.GetLocation().Y, ZCorrected.GetLocation().Z));

	FTransform PositionCorrected = UKismetMathLibrary::TLerp(AddTransformByComponents(WallTargetTransform, XYZCorrected), WallTargetTransform, PositionAlpha);
	FTransform BlendCorrected = UKismetMathLibrary::TLerp(AddTransformByComponents(WallTargetTransform, WallClimbActualStartOffset), PositionCorrected, Blend);

	TargetRotation = FRotator(BlendCorrected.GetRotation());
	SetActorLocationAndRotation(BlendCorrected.GetLocation(), FRotator(BlendCorrected.GetRotation()));
}

void ALocomotionCharacter::WallClimbEnd()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ALocomotionCharacter::WallClimbTimelineCallback(float Value)
{
	WallClimbUpdate(Value);
}

void ALocomotionCharacter::SetCharacterRotation(FRotator NewTargetRotation, bool ShouldRotationBeInterpolated, float InterpSpeed)
{
	TargetRotation = NewTargetRotation;
	TargetCharacterRotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(NewTargetRotation, CharacterRotation).Yaw;
	if (ShouldRotationBeInterpolated && InterpSpeed != 0.0f)
	{
		CharacterRotation = UKismetMathLibrary::RInterpTo(CharacterRotation, NewTargetRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
	}
	else
	{
		CharacterRotation = NewTargetRotation;
	}
	SetActorRotation(CharacterRotation);
}

void ALocomotionCharacter::SetCharacterRotationWithLimit(float AimYawLimt, float InterpSpeed)
{
	if (abs(AimYawDelta) > AimYawLimt)
	{
		if (AimYawDelta > 0.0f)
		{
			SetCharacterRotation(FRotator(0.0f, LookingRotation.Yaw - AimYawLimt, 0.0f), true, InterpSpeed);
		}
		else
		{
			SetCharacterRotation(FRotator(0.0f, LookingRotation.Yaw + AimYawLimt, 0.0f), true, InterpSpeed);
		}
	}
}

float ALocomotionCharacter::CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate)
{
	FVector ChosenVelocity = ChooseVelocity();
	float LocalVelocityVectorLength = FVector(ChosenVelocity.X, ChosenVelocity.Y, 0.0f).Size();
	float LocalRotationRate = 0.0f;
	if (LocalVelocityVectorLength > SlowSpeed)
	{
		LocalRotationRate = UKismetMathLibrary::MapRangeClamped(LocalVelocityVectorLength, SlowSpeed, FastSpeed, SlowSpeedRate, FastSpeedRate);
	}
	else
	{
		LocalRotationRate = UKismetMathLibrary::MapRangeClamped(LocalVelocityVectorLength, 0.0f, SlowSpeed, 0.0f, SlowSpeedRate);
	}

	LocalRotationRate = UKismetMathLibrary::FClamp((LocalRotationRate * RotationMultiplier), 0.1f, 15.0f);

	if (RotationMultiplier != 1.0f)
	{
		RotationMultiplier = UKismetMathLibrary::FClamp((RotationMultiplier + GetWorld()->GetDeltaSeconds()), 0.0f, 1.0f);
	}

	return LocalRotationRate;
}

FRotator ALocomotionCharacter::LookingDirectionWithOffset(float OffsetInterpSpeed, float NEAngle, float NWAngle, float SEAngle, float SWAngle, float Buffer)
{
	float LocalValueToTest = 0.0f;
	if (bHasMovementInput)
	{
		LocalValueToTest = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LookingRotation).Yaw;
	}
	else
	{
		LocalValueToTest = UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, LookingRotation).Yaw;
	}

	float LocalRotationOffsetInterpTarget = 0.0f;
	if (bIsAiming)
	{
		switch (CardinalDirection) 
		{
		case ECardinalDirection::North:
			LocalRotationOffsetInterpTarget = LocalValueToTest;
			break;

		case ECardinalDirection::East:
			LocalRotationOffsetInterpTarget = LocalValueToTest - 90.0f;
			break;

		case ECardinalDirection::West:
			LocalRotationOffsetInterpTarget = LocalValueToTest + 90.0f;
			break;

		case ECardinalDirection::South:
			if (LocalValueToTest > 0.0f)
			{
				LocalRotationOffsetInterpTarget = LocalValueToTest - 180.0f;
			}
			else
			{
				LocalRotationOffsetInterpTarget = LocalValueToTest + 180.0f;
			}
			break;
		}
	}
	else
	{
		if (Gait == EGait::Walking)
		{
			LocalRotationOffsetInterpTarget = 0.0f;
		}
		else if (Gait == EGait::Running || Gait == EGait::Sprinting)
		{
			switch (CardinalDirection)
			{
			case ECardinalDirection::North:
				LocalRotationOffsetInterpTarget = LocalValueToTest;
				break;

			case ECardinalDirection::East:
				LocalRotationOffsetInterpTarget = LocalValueToTest - 90.0f;
				break;

			case ECardinalDirection::West:
				LocalRotationOffsetInterpTarget = LocalValueToTest + 90.0f;
				break;

			case ECardinalDirection::South:
				if (LocalValueToTest > 0.0f)
				{
					LocalRotationOffsetInterpTarget = LocalValueToTest - 180.0f;
				}
				else
				{
					LocalRotationOffsetInterpTarget = LocalValueToTest + 180.0f;
				}
				break;
			}
		}
	}

	if (IsPassedCardinalDirectionInRange(LocalValueToTest, NWAngle, NEAngle, Buffer, ECardinalDirection::North))
	{
		CardinalDirection = ECardinalDirection::North;
	}
	else if (IsPassedCardinalDirectionInRange(LocalValueToTest, NEAngle, SEAngle, Buffer, ECardinalDirection::East))
	{
		CardinalDirection = ECardinalDirection::East;
	}
	else if (IsPassedCardinalDirectionInRange(LocalValueToTest, SWAngle, NWAngle, Buffer, ECardinalDirection::West))
	{
		CardinalDirection = ECardinalDirection::West;
	}
	else
	{
		CardinalDirection = ECardinalDirection::South;
	}

	RotationOffset = UKismetMathLibrary::FInterpTo(RotationOffset, LocalRotationOffsetInterpTarget, GetWorld()->GetDeltaSeconds(), OffsetInterpSpeed);

	return FRotator(0.0f, LookingRotation.Yaw + RotationOffset, 0.0f);
}

bool ALocomotionCharacter::IsPassedCardinalDirectionInRange(float Value, float Min, float Max, float Buffer, ECardinalDirection CardinalDirectionToTest)
{
	if (CardinalDirectionToTest == CardinalDirection)
	{
		return UKismetMathLibrary::InRange_FloatFloat(Value, (Min - Buffer), (Max + Buffer), true, true);
	}
	else
	{
		return UKismetMathLibrary::InRange_FloatFloat(Value, (Min + Buffer), (Max - Buffer), true, true);
	}
}

void ALocomotionCharacter::CalculateActorLocationAndRotationWhileRagdolling(FRotator RagdollRotation, FVector CurrentRagdollLocation, FRotator & RagdollRotationOut, FVector & RagdollLocationOut)
{
	FVector LocalTraceEnd = FVector(CurrentRagdollLocation.X, CurrentRagdollLocation.Y, (CurrentRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	FHitResult Hit;
	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CurrentRagdollLocation, LocalTraceEnd, ECC_Visibility, TraceParams);

	bRagdollOnGround = bHit;
	if (bHit)
	{
		RagdollLocationOut = FVector(CurrentRagdollLocation.X, CurrentRagdollLocation.Y, (CurrentRagdollLocation.Z + 2.0f + (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - abs(Hit.ImpactPoint.Z - Hit.TraceStart.Z))));
	}
	else
	{
		RagdollLocationOut = CurrentRagdollLocation;
	}

	if (RagdollRotation.Roll > 0.0f)
	{
		RagdollRotationOut = FRotator(0.0f, RagdollRotation.Yaw, 0.0f);
	}
	else
	{
		RagdollRotationOut = FRotator(0.0f, RagdollRotation.Yaw - 180.0f, 0.0f);
	}
}

void ALocomotionCharacter::CalculateEssentialVariables()
{
	FVector ChosenVelocity = ChooseVelocity();
	FVector LocalVector = FVector(ChosenVelocity.X, ChosenVelocity.Y, 0.0f);
	bIsMoving = UKismetMathLibrary::NotEqual_VectorVector(ChosenVelocity, FVector(0.0f, 0.0f, 0.0f), 0.1f);
	if (bIsMoving)
	{
		LastVelocityRotation = UKismetMathLibrary::MakeRotFromX(ChosenVelocity);
		Direction = (UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, CharacterRotation)).Yaw;
	}
	MovementInput = GetCharacterMovement()->GetLastInputVector();
	bHasMovementInput = UKismetMathLibrary::NotEqual_VectorVector(MovementInput, FVector(0.0f, 0.0f, 0.0f), 0.0001f);
	if (bHasMovementInput)
	{
		LastMovementInputRotation = UKismetMathLibrary::MakeRotFromX(MovementInput);
		MovementInputVelocityDifference = (UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LastVelocityRotation)).Yaw;
	}

	float PrevAimYawLocal = LookingRotation.Yaw;
	LookingRotation = SetLookingRotation();
	AimYawRate = (LookingRotation.Yaw - PrevAimYawLocal) / GetWorld()->GetDeltaSeconds();
	AimYawDelta = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, CharacterRotation).Yaw;
}

void ALocomotionCharacter::CustomAcceleration()
{
	GetCharacterMovement()->MaxAcceleration = (UKismetMathLibrary::MapRangeClamped(abs(MovementInputVelocityDifference), 45.0f, 130.0f, 1.0f, 0.2f)) * RunningAcceleration;
	GetCharacterMovement()->GroundFriction = (UKismetMathLibrary::MapRangeClamped(abs(MovementInputVelocityDifference), 45.0f, 130.0f, 1.0f, 0.4f)) * RunningFriction;
}

void ALocomotionCharacter::ManageCharacterRotation()
{
	switch (MovementType)
	{
	case EMovementType::Falling:
		if (RotationMode == ERotationMode::VelocityDirection && bIsMoving)
		{
			SetCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, 10.0f);
		}
		else if (RotationMode == ERotationMode::TargetDirection)
		{
			JumpRotation = LookingRotation;
			SetCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, 10.0f);
		}
		break;

	case EMovementType::Grounded:
		if (bIsMoving)
		{
			FRotator LocalTargetRotation = LookingDirectionWithOffset(5.0f, 60.0f, -60.0f, 120.0f, -120.0f, 5.0f);
			if (RotationMode == ERotationMode::TargetDirection)
			{
				SetCharacterRotation(LocalTargetRotation, true, CalculateRotationRate(165.0f, 5.0f, 375.0f, 10.0f));
			}
			else if (RotationMode == ERotationMode::VelocityDirection)
			{
				SetCharacterRotation(FRotator(0.0f, LastVelocityRotation.Yaw, 0.0f), true, CalculateRotationRate(165.0f, 5.0f, 375.0f, 10.0f));
			}
		}
		else if (!IsPlayingRootMotion() && RotationMode == ERotationMode::TargetDirection && bIsAiming)
		{
			SetCharacterRotationWithLimit(90.0f, 15.0f);
		}
		break;
	}
}

void ALocomotionCharacter::SprintCheck()
{
	// if (Gait == EGait::Walking) return;
	if (bShouldSprint && CanSprint())
	{
		if (bGateOneOpen)
		{
			SetGait(EGait::Sprinting);
			bGateOneOpen = false;
			bGateTwoOpen = true;
		}
	}
	else if (!bShouldSprint && CanSprint())
	{
		if (bGateTwoOpen)
		{
			SetGait(EGait::Running);
			bGateOneOpen = true;
			bGateTwoOpen = false;
		}
	}
}

bool ALocomotionCharacter::CanSprint()
{
	switch (MovementType)
	{
	case EMovementType::Ragdoll:
		return false;

	case EMovementType::Grounded:
		switch (RotationMode)
		{
		case ERotationMode::VelocityDirection:
			return true;

		case ERotationMode::TargetDirection:
			if (bHasMovementInput)
			{
				if (bIsAiming)
				{
					return true;
				}
				else
				{
					return abs((UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LookingRotation)).Yaw) < 50.0f;
				}
			}
			else
			{
				return true;
			}
			break;
		}
	}
	
	return false;
}

void ALocomotionCharacter::OnLandedTimerHandleEnd()
{
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
}

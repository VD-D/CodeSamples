// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/AutobattlerAIController.h"

/* Autobattler includes. */
#include "AI/GetTarget.h"
#include "AI/GetTargetDerived/GetSelfAsTarget.h"
#include "Core/AutobattlerManager.h"
#include "DataAssets/AutobattlerConfiguration.h"
#include "DataAssets/AutobattlerAbility.h"
#include "DataAssets/AutobattlerAttack.h"
#include "DataAssets/AutobattlerSkill.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Game/Skills/ExecuteSkill.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EQSRenderingComponent.h"
#include "Navigation/CrowdFollowingComponent.h"

AAutobattlerAIController::AAutobattlerAIController(const FObjectInitializer& ObjectInitializer) 
// Note R.U. To be restored at later date if necessary.
// : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
    CurrentPhase = EAutobattlerPhase::Setup; 
    ShouldUpdate = true;
    PrimaryActorTick.bCanEverTick = true;

    RenderComp = CreateDefaultSubobject<UEQSRenderingComponent>(TEXT("EQSRenderComp"));
}

void AAutobattlerAIController::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        // Note R.U. To be restored at later date if necessary.
        /*
        if (UCrowdFollowingComponent* CrowdFollowingComponent = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
        {
            CrowdFollowingComponent->SetCrowdSimulationState(ECrowdSimulationState::ObstacleOnly);
            CrowdFollowingComponent->SetCrowdObstacleAvoidance(true);
            CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1000.0f);
            CrowdFollowingComponent->SetCrowdSeparation(true);
            CrowdFollowingComponent->SetCrowdAnticipateTurns(true);
            CrowdFollowingComponent->SetCrowdOptimizeTopology(true);
            CrowdFollowingComponent->SetCrowdRotateToVelocity(true);
            CrowdFollowingComponent->SetCrowdSeparationWeight(3.0f);
            CrowdFollowingComponent->SetCrowdCollisionQueryRange(600.0f);
        }
        */

        if (AAutobattlerManager* Manager = AAutobattlerManager::GetManager(this))
		{
			Manager->OnGamePhaseAdvanced.AddDynamic(this, &AAutobattlerAIController::OnGamePhaseChanged);
		}
        else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [BeginPlay] Could not get Game Subsystem!"), *GetName()));

        bool PreferNativeUpdate = true;
        if (const UAutobattlerConfiguration* Configuration = UAutobattlerConfiguration::GetConfigurationAsset(this))
        {
            if (Configuration->UseBehaviorTree && IsValid(Configuration->AutobattlerBehaviorTree))
            {
                PreferNativeUpdate = !RunBehaviorTree(Configuration->AutobattlerBehaviorTree);
            }
        }

        if (PreferNativeUpdate)
        {
            FTimerHandle Handle;
            FTimerDelegate Delegate;
            Delegate.BindUObject(this, &AAutobattlerAIController::AIUpdate);

            GetWorldTimerManager().SetTimer(Handle, Delegate, 0.2f, true);
        }

        FTimerHandle DebugHandle;
        FTimerDelegate DebugDelegate;
        DebugDelegate.BindUObject(this, &AAutobattlerAIController::ResetCanDrawDebugItems);

        GetWorldTimerManager().SetTimer(DebugHandle, DebugDelegate, RedrawQueryItemsFrequenty, true);
    }
}

void AAutobattlerAIController::SetDebugEnabled(bool NewEnabled)
{
    IsDebugEnabled = NewEnabled;
    LastQueryResultType = ELastQueryResultType::None;
    LastQueryResult = nullptr;
    QueryInstance = nullptr;
}

bool AAutobattlerAIController::GetAIShouldEverUpdate() const
{
    return CurrentPhase == EAutobattlerPhase::Fight && ShouldUpdate && IsValid(GetControlledCharacter()) && GetControlledCharacter()->GetCurrentAction() != EActionType::Dead;
}

FAbilityTargetingProperties AAutobattlerAIController::GetAbilityTargetingProperties(const UAutobattlerSkill* Skill)
{
    if (Skill == nullptr) return FAbilityTargetingProperties();
	if (Skill->GetTargetImplementationClass.Get() == nullptr && Skill->PrimaryTargetFilter != EAbilityFilterType::Self)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(
			FString::Printf(TEXT("%s : [GetAbilityTargetingProperties] %s Does not have valid GetTarget class!"),
				*GetName(),
				*Skill->GetName()
			));
		return FAbilityTargetingProperties();
	}

	AAutobattlerCharacter* TargetCharacter = nullptr;
	FVector TargetLocation = FVector::ZeroVector;
	TArray<AAutobattlerCharacter*> FilteredCharacters;
    ESkillTargetingMode SkillTargetingMode = ESkillTargetingMode::None;
    if (Skill->PrimaryTargetFilter == EAbilityFilterType::Self)
    {
        UGetTarget* GetTargetImplementation = NewObject<UGetSelfAsTarget>(this, UGetSelfAsTarget::StaticClass());
        SkillTargetingMode = GetTargetImplementation->GetTarget(GetControlledCharacter(), FilteredCharacters, TargetCharacter, TargetLocation);
    }
    else
    {
        UAutobattlerFunctionLibrary::GetCharactersFiltered(FilteredCharacters, GetControlledCharacter(), Skill->PrimaryTargetFilter);

	    UGetTarget* GetTargetImplementation = NewObject<UGetTarget>(this, Skill->GetTargetImplementationClass);
	    SkillTargetingMode = GetTargetImplementation->GetTarget(GetControlledCharacter(), FilteredCharacters, TargetCharacter, TargetLocation);
    }

    float ModifiedRange = 0.0f; 
    if (Cast<UAutobattlerAbility>(Skill) != nullptr) ModifiedRange = GetControlledCharacter()->GetAbilityRangeModifier();
    else if (Cast<UAutobattlerAttack>(Skill) != nullptr) ModifiedRange = GetControlledCharacter()->GetAttackRangeModifier();
    const float FinalRange = Skill->Range + ModifiedRange;

	FAbilityTargetingProperties NewTargetingProperties;
	NewTargetingProperties.TargetCharacter = TargetCharacter;
	NewTargetingProperties.TargetLocation = TargetLocation;
	NewTargetingProperties.TargetingMode = SkillTargetingMode;
	NewTargetingProperties.Range = FinalRange;

	return NewTargetingProperties;
}

const UAutobattlerSkill* AAutobattlerAIController::GetRelevantSkill(EAbilityType SkillType)
{
    if (FAutobattlerCharacterDefinition* CharacterDefinition = UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(this, CharacterListingName))
	{
        if (SkillType == EAbilityType::Skill) return CharacterDefinition->AbilityImplementation;
		else if (SkillType == EAbilityType::Attack) return CharacterDefinition->AttackImplementation;
	}
	else
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(
			FString::Printf(TEXT("%s : [GetRelevantSkill] Character Definition with row name %s does not exist (may have not been initialised)!"),
				*GetName(),
				*CharacterListingName.ToString()
			)
		);
	}

	return nullptr;
}

void AAutobattlerAIController::OnGamePhaseChanged(EAutobattlerPhase NewGamePhase)
{
    if (!HasAuthority()) return;

    CurrentPhase = NewGamePhase;
    if (NewGamePhase == EAutobattlerPhase::Fight)
    {
        if (FAutobattlerCharacterDefinition* CharacterDefinition = UAutobattlerFunctionLibrary::GetCharacterDefinitionFromConfigurationDatatable(this, CharacterListingName))
	    {
            if (const UAutobattlerAbility* Ability = CharacterDefinition->AbilityImplementation)
            {
                if (Ability->AbilityTriggerType == EAbilityTriggerType::Cooldown)
                {
                    if (Ability->Cooldown <= 0.0f) UsesAbilityOnly = true;
                    else
                    {
                        if (Ability->StartOffCooldown) IsAbilityQueued = true;

                        FTimerHandle Handle;
                        FTimerDelegate Delegate;
                        Delegate.BindUFunction(this, FName("OnAbilityCooldownEnd"));

                        GetWorldTimerManager().SetTimer(Handle, Delegate, FMath::Max(Ability->Cooldown, 1.0f), false);
                    }
                }
                else if (Ability->AbilityTriggerType == EAbilityTriggerType::Charge)
                {
                    AAutobattlerCharacter* OwnerCharacter = GetControlledCharacter();
                    if (ensureAlwaysMsgf(IsValid(OwnerCharacter), TEXT("%s : [OnGamePhaseChanged] When configuring charge component, controlled pawn is not an Autobattler Character!"), *GetName()))
                    {
                        OwnerCharacter->ConfigureGetChargeComponent(Ability->ChargeComponentClass, Ability->ChargesNeededForTrigger);
                    }
                }
            }
            else
            {
                UAutobattlerFunctionLibrary::PrintErrorToLog(
			        FString::Printf(TEXT("%s : [OnGamePhaseChanged] Failed to find ability implementation, character will not use any ability!"),
				        *GetName()
			        )
		        );
            }
	    }
        AIUpdate();
    }
    else
    {
        StopMovement();
        IsMovingToTarget = false;
        ShouldUpdate = false;
    }
}

void AAutobattlerAIController::AIUpdate()
{
    // EQS implementation
    
    if (GetAIShouldEverUpdate())
    {
        bool InRange = false;

        EAbilityType RelevantAbilityType = IsAbilityQueued || UsesAbilityOnly ? EAbilityType::Skill : EAbilityType::Attack;
        const UAutobattlerSkill* RelevantSkill = GetRelevantSkill(RelevantAbilityType);
        TargetingProperties = GetAbilityTargetingProperties(RelevantSkill);

        if (TargetingProperties.TargetingMode == ESkillTargetingMode::None || !IsValid(GetControlledCharacter())) ResetMovementState();
        if (TargetingProperties.TargetingMode == ESkillTargetingMode::Actor)
	    {
		    if (!IsValid(TargetingProperties.TargetCharacter))
            {
                ResetMovementState();
                return;
            }

            if (TargetingProperties.TargetCharacter != PreviousTargetingProperties.TargetCharacter) ResetMovementState();

		    float CartesianDistance = UAutobattlerFunctionLibrary::CartesianDistance(GetControlledCharacter()->GetActorLocation(), TargetingProperties.TargetCharacter->GetActorLocation());
		    InRange = CartesianDistance < TargetingProperties.Range + TargetingProperties.TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	    }
	    else if (TargetingProperties.TargetingMode == ESkillTargetingMode::Location)
	    {
            if (!TargetingProperties.TargetLocation.Equals(PreviousTargetingProperties.TargetLocation, 1.0f)) ResetMovementState();

            SetFocalPoint(TargetingProperties.TargetLocation);
		    float CartesianDistance = UAutobattlerFunctionLibrary::CartesianDistance(GetControlledCharacter()->GetActorLocation(), TargetingProperties.TargetLocation);
		    InRange = CartesianDistance < TargetingProperties.Range;
	    }

        if (!InRange && !IsMovingToTarget)
        {
            // TODO: How do we move to a target location rather than target actor?
            if (!IsValid(TargetingProperties.TargetCharacter)) return;
            if (UEnvQuery* SurroundingPointsQuery = TargetingProperties.TargetCharacter->GetSurroundingPointsQuery())
            {
                FEnvQueryRequest EQSRequest = FEnvQueryRequest(SurroundingPointsQuery, this);
                EQSRequest.Execute(EEnvQueryRunMode::Type::SingleResult, this, &AAutobattlerAIController::SurroundingPointsQueryFinished);
            }
        }
        else if (InRange)
        {
            if (IsDebugEnabled)
            {
                LastQueryResultType = ELastQueryResultType::None;
                LastQueryResult = nullptr;
                QueryInstance = nullptr;
            }

            if (TargetingProperties.TargetCharacter != GetControlledCharacter()) SetFocus(TargetingProperties.TargetCharacter);

            ResetMovementState();
            ShouldUpdate = false;
            StartExecuteSkill(RelevantSkill);
        }

        PreviousTargetingProperties = TargetingProperties;
    }
}

void AAutobattlerAIController::OnAbilityCooldownEnd()
{
    IsAbilityQueued = true;
}

void AAutobattlerAIController::PathAroundQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (FEnvQueryResult* QueryResult = Result.Get())
    {
        UE_LOG(LogTemp, Log, TEXT("%s returned %d items when running path around query"), *GetName(), QueryResult->Items.Num());
        if (QueryResult->Items.IsValidIndex(0))
        {
            MoveToLocation(QueryResult->GetItemAsLocation(0), 1.0f, false); // TODO: Configuration.
            DesiredMoveToLocation = QueryResult->GetItemAsLocation(0);
            SetFocalPoint(DesiredMoveToLocation);

            if (IsDebugEnabled)
            {
                LastQueryResult = QueryResult;
                LastQueryResultType = ELastQueryResultType::PathAround;

                if (UEnvQueryManager* EQS = UEnvQueryManager::GetCurrent(GetWorld()))
                {
                    FEnvQueryRequest QueryRequest(TargetingProperties.TargetCharacter->GetPathAroundQuery(), this);
                    QueryInstance = EQS->PrepareQueryInstance(QueryRequest, EEnvQueryRunMode::Type::AllMatching);
                    if (QueryInstance.IsValid()) EQS->RegisterExternalQuery(QueryInstance);
                }

                DrawEnvQueryItems(QueryResult);
            }
        }
        else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [PathAroundQueryFinished] Query did not return any items!"), *GetName()));
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [PathAroundQueryFinished] Query result is invalid!"), *GetName()));
}

void AAutobattlerAIController::SurroundingPointsQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (FEnvQueryResult* QueryResult = Result.Get())
    {
        UE_LOG(LogTemp, Log, TEXT("%s returned %d items when running surrounding points query"), *GetName(), QueryResult->Items.Num());
        if (QueryResult->Items.IsValidIndex(0))
        {
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(GetPawn());
            if (TargetingProperties.TargetingMode == ESkillTargetingMode::Actor && IsValid(TargetingProperties.TargetCharacter))
            {
                QueryParams.AddIgnoredActor(TargetingProperties.TargetCharacter);
            }
            QueryParams.bTraceComplex = false;

            FHitResult HitResult;
            bool Hit = GetWorld()->SweepSingleByProfile(
                HitResult,
                GetPawn()->GetActorLocation(),
                Result->GetItemAsLocation(0),
                FQuat::Identity,
                FName("Pawn"), // TODO: Configuration
                FCollisionShape::MakeSphere(34.0f), // TODO: Configuration
                QueryParams
            );

            if (IsDebugEnabled)
            {
                const FVector TraceVec = Result->GetItemAsLocation(0) - GetPawn()->GetActorLocation();
                const float Dist = TraceVec.Size();

                const FVector Center = GetPawn()->GetActorLocation() + TraceVec * 0.5f;
                const float HalfHeight = (Dist * 0.5f) + 34.0f; // TODO: 34.0f ; Configuration.
                const FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();

                DrawDebugCapsule(GetWorld(), Center, HalfHeight, 34.0f, CapsuleRot, FColor::Magenta, false, 0.2f, 0, 3.0f); // TODO: Configuration
                if (Hit) DrawDebugSphere(GetWorld(), HitResult.Location, 34.0f, 24, FColor::Orange, false, 0.2f, 0, 3.0f); // TODO: Configuration
            }

            if (Hit)
            {
                // TODO: How do we move to a target location rather than target actor?
                if (UEnvQuery* PathAroundQuery = TargetingProperties.TargetCharacter->GetPathAroundQuery())
                {
                    FEnvQueryRequest EQSRequest = FEnvQueryRequest(PathAroundQuery, this);
                    EQSRequest.Execute(EEnvQueryRunMode::Type::SingleResult, this, &AAutobattlerAIController::PathAroundQueryFinished);
                }
            }
            else
            {
                // Alternative version where we move to the target rather than the surrounding location.
                // MoveToLocation(Result->GetItemAsLocation(0), 5.0f); // TODO: Configuration.

                if (IsDebugEnabled)
                {
                    LastQueryResult = QueryResult;
                    LastQueryResultType = ELastQueryResultType::PointsAround;

                    if (UEnvQueryManager* EQS = UEnvQueryManager::GetCurrent(GetWorld()))
                    {
                        FEnvQueryRequest QueryRequest(TargetingProperties.TargetCharacter->GetSurroundingPointsQuery(), this);
                        QueryInstance = EQS->PrepareQueryInstance(QueryRequest, EEnvQueryRunMode::Type::AllMatching);
                        if (QueryInstance.IsValid()) EQS->RegisterExternalQuery(QueryInstance);
                    }

                    DrawEnvQueryItems(QueryResult);
                }
                
                if (TargetingProperties.TargetingMode == ESkillTargetingMode::Actor)
                {
                    MoveToActor(TargetingProperties.TargetCharacter, 5.0f); // TODO: Configuration.
                    DesiredMoveToLocation = TargetingProperties.TargetCharacter->GetActorLocation();
                    if (TargetingProperties.TargetCharacter != GetControlledCharacter()) SetFocus(TargetingProperties.TargetCharacter);
                }
                else if (TargetingProperties.TargetingMode == ESkillTargetingMode::Location) 
                {
                    MoveToLocation(TargetingProperties.TargetLocation, 5.0f); // TODO: Configuration.
                    DesiredMoveToLocation = TargetingProperties.TargetLocation;
                    SetFocalPoint(TargetingProperties.TargetLocation);
                }
                
            }
        }
        else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SurroundingPointsQueryFinished] Query returned no results!"), *GetName()));
    }
    else UAutobattlerFunctionLibrary::PrintErrorToLog(FString::Printf(TEXT("%s : [SurroundingPointsQueryFinished] Query result is invalid!"), *GetName()));
}

void AAutobattlerAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAutobattlerAIController::ResetState()
{
    if (CurrentPhase != EAutobattlerPhase::Fight) return;

    if (LastRelevantActionType == EAbilityType::Skill) IsAbilityQueued = false;
    ShouldUpdate = true;
    AIUpdate();
}

void AAutobattlerAIController::ResetMovementState()
{
    StopMovement();
    IsMovingToTarget = false;
}

void AAutobattlerAIController::TryExecuteRelevantSkill()
{
    EAbilityType RelevantAbilityType = IsAbilityQueued || UsesAbilityOnly ? EAbilityType::Skill : EAbilityType::Attack;
    const UAutobattlerSkill* RelevantSkill = GetRelevantSkill(RelevantAbilityType);
    
    ShouldUpdate = false;
    StartExecuteSkill(RelevantSkill);
}

void AAutobattlerAIController::StartExecuteSkill(const UAutobattlerSkill* SkillToExecute)
{
    if (IsValid(SkillToExecute))
    {
        if (AAutobattlerCharacter* ParentCharacter = GetControlledCharacter())
        {
            EActionType RelevantActionType = IsAbilityQueued ? EActionType::UsingSkill : EActionType::Attacking;
            LastRelevantActionType = RelevantActionType == EActionType::UsingSkill ? EAbilityType::Skill : EAbilityType::Attack;
            ParentCharacter->SetActionType(RelevantActionType);
            ParentCharacter->ReplicateExecuteAbilityAnimations(
                SkillToExecute->SkillAnimations[FMath::RandRange(0, SkillToExecute->SkillAnimations.Num() - 1)],
                SkillToExecute->ActionSpeed,
                RelevantActionType
            );
        }
        else ResetState();
    }
    else ResetState();
}

void AAutobattlerAIController::ExecuteSkill(EAbilityType AbilityTypeTriggered)
{
    if (CurrentPhase != EAutobattlerPhase::Fight) return;
    const UAutobattlerSkill* Skill = GetRelevantSkill(AbilityTypeTriggered);
    if (const UAutobattlerAbility* Ability = Cast<UAutobattlerAbility>(Skill)) IsAbilityQueued = false;
	if (Skill == nullptr) 
	{
		ResetState();
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Skill->ExecuteSkillClass.Get() == nullptr)
	{
		UAutobattlerFunctionLibrary::PrintErrorToLog(
			FString::Printf(TEXT("%s : [ExecuteAbility] %s does not have valid ExecuteSkillClass!"),
			*GetName(),
			*Skill->GetName()
		));
        ResetState();
		return;
	}

	FTransform SkillTransform;
	GetNextSkillSpawnLocation(SkillTransform, Skill->OnSkillTriggerEnd, Skill->ProjectileSocketSpawnName);

	AExecuteSkill* SkillActor = GetWorld()->SpawnActor<AExecuteSkill>(
		Skill->ExecuteSkillClass,
		SkillTransform,
		ActorSpawnParams
	);

	if (Skill->OnSkillTriggerEnd == ESkillTriggerEnd::Instant) SkillActor->InitialiseAsInstant(Skill, GetControlledCharacter(), PreviousTargetingProperties);
	else if (Skill->OnSkillTriggerEnd == ESkillTriggerEnd::Projectile) SkillActor->InitialiseAsProjectile(Skill, GetControlledCharacter(), PreviousTargetingProperties);
	else if (Skill->OnSkillTriggerEnd == ESkillTriggerEnd::Effect) SkillActor->InitialiseAsEffect(Skill, GetControlledCharacter(), PreviousTargetingProperties);

    if (const UAutobattlerAbility* Ability = Cast<UAutobattlerAbility>(Skill))
    {
        if (Ability->AbilityTriggerType == EAbilityTriggerType::Cooldown)
        {
            FTimerHandle Handle;
            FTimerDelegate Delegate;
            Delegate.BindUFunction(this, FName("OnAbilityCooldownEnd"));

            GetWorldTimerManager().SetTimer(Handle, Delegate, FMath::Max(Ability->Cooldown, 1.0f), false);
        }
    }

    OnSkillExecuted.Broadcast(LastRelevantActionType);
}

void AAutobattlerAIController::EndSkill()
{
    if (AAutobattlerCharacter* ParentCharacter = GetControlledCharacter()) ParentCharacter->SetAnimationActionType(EActionType::Idle);
    if (LastRelevantActionType == EAbilityType::Skill) IsAbilityQueued = false;
}

AAutobattlerCharacter* AAutobattlerAIController::GetControlledCharacter() const
{
    return Cast<AAutobattlerCharacter>(GetPawn());
}

void AAutobattlerAIController::GetNextSkillSpawnLocation(FTransform& Transform, ESkillTriggerEnd SkillTriggerType, const FName& SocketName)
{
    AAutobattlerCharacter* ControlledCharacter = GetControlledCharacter();
    if (SkillTriggerType == ESkillTriggerEnd::Instant)
	{
		if (PreviousTargetingProperties.TargetingMode == ESkillTargetingMode::Actor && IsValid(PreviousTargetingProperties.TargetCharacter))
		{
			Transform.SetLocation(PreviousTargetingProperties.TargetCharacter->GetActorLocation());
		}
		else if (PreviousTargetingProperties.TargetingMode == ESkillTargetingMode::Location)
		{
			Transform.SetLocation(PreviousTargetingProperties.TargetLocation);
		}
		else
		{
			Transform.SetLocation(IsValid(ControlledCharacter) ? ControlledCharacter->GetActorLocation() : FVector::ZeroVector);
		}
	}
	else if (SkillTriggerType == ESkillTriggerEnd::Projectile)
	{
		if (IsValid(ControlledCharacter) && ControlledCharacter->GetMesh()->DoesSocketExist(SocketName)) Transform.SetLocation(ControlledCharacter->GetMesh()->GetSocketLocation(SocketName));
		else Transform.SetLocation(IsValid(ControlledCharacter) ? ControlledCharacter->GetActorLocation() : FVector::ZeroVector);
	}
	else if (SkillTriggerType == ESkillTriggerEnd::Effect)
	{
		if (PreviousTargetingProperties.TargetingMode == ESkillTargetingMode::Actor && IsValid(PreviousTargetingProperties.TargetCharacter))
		{
			Transform.SetLocation(PreviousTargetingProperties.TargetCharacter->GetActorLocation());
		}
		else if (PreviousTargetingProperties.TargetingMode == ESkillTargetingMode::Location)
		{
			Transform.SetLocation(PreviousTargetingProperties.TargetLocation);
		}
		else Transform.SetLocation(FVector::ZeroVector);
	}
}

void AAutobattlerAIController::DrawEnvQueryItems(FEnvQueryResult* QueryResult)
{
    if (!IsDebugEnabled || QueryResult == nullptr || !CanDrawDebugItems) return;
    CanDrawDebugItems = false;

    for (int32 i = 0; i < QueryResult->Items.Num(); i++)
    {
        DrawDebugSphere(
            GetWorld(),
            QueryResult->GetItemAsLocation(i),
            16.0f, // TODO : Configuration
            24,
            QueryResult->GetItemScore(i) > 0.0f ? FColor::Green : FColor::Blue,
            false,
            RedrawQueryItemsFrequenty,
            0,
            3.0f
        );

        if (i >= MaxItemsToDraw) break;
    }
}

void AAutobattlerAIController::ResetCanDrawDebugItems()
{
    CanDrawDebugItems = true;
}

const FEnvQueryResult* AAutobattlerAIController::GetQueryResult() const
{
    if (!IsDebugEnabled) return nullptr;

    if (LastQueryResultType != ELastQueryResultType::None)
    {
        return LastQueryResult;
    }

    return nullptr;
}

const FEnvQueryInstance* AAutobattlerAIController::GetQueryInstance() const
{
    if (!IsDebugEnabled) return nullptr;

    if (LastQueryResultType != ELastQueryResultType::None)
    {
        return &(*QueryInstance.Get());
    }

    return nullptr;
}

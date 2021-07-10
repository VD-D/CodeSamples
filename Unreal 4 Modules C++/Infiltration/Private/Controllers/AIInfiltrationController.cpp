// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/AIInfiltrationController.h"
#include "Libraries/InfiltrationUtilityLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIInfiltrationController::AAIInfiltrationController()
{
    /* Component creation */
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
    AIPeripheralPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Peripheral Perception"));

    AISightConfiguration = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Configuration"));
    AIHearingConfiguration = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AI Hearing Configuration"));
    AIPeripheralSightConfiguration = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Peripheral Sight Configuration"));

    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));

    /* Default forward sight setup. */
    AISightConfiguration->SightRadius = 2000.0f;
    AISightConfiguration->LoseSightRadius = 0.0f;
    AISightConfiguration->SetMaxAge(0.01f);
    AISightConfiguration->PeripheralVisionAngleDegrees = 190.0f;
    AISightConfiguration->DetectionByAffiliation.bDetectEnemies = true;
    AISightConfiguration->DetectionByAffiliation.bDetectFriendlies = false;
    AISightConfiguration->DetectionByAffiliation.bDetectNeutrals = false; 

    /* Default hearing setup. */
    AIHearingConfiguration->HearingRange = 3000.0f;
    AIHearingConfiguration->SetMaxAge(0.01f);
    AIHearingConfiguration->DetectionByAffiliation.bDetectEnemies = true;
    AIHearingConfiguration->DetectionByAffiliation.bDetectFriendlies = true;
    AIHearingConfiguration->DetectionByAffiliation.bDetectNeutrals = true;

    /* Default peripheral sight setup. */
    AIPeripheralSightConfiguration->SightRadius = 1200.0f;
    AIPeripheralSightConfiguration->LoseSightRadius = 0.0f;
    AIPeripheralSightConfiguration->SetMaxAge(0.01f);
    AIPeripheralSightConfiguration->PeripheralVisionAngleDegrees = 360.0f;
    AIPeripheralSightConfiguration->DetectionByAffiliation.bDetectEnemies = true;
    AIPeripheralSightConfiguration->DetectionByAffiliation.bDetectFriendlies = false;
    AIPeripheralSightConfiguration->DetectionByAffiliation.bDetectNeutrals = false;

    /* Configuration of AI Perception. */
    AIPerception->ConfigureSense(*AISightConfiguration);
    AIPerception->ConfigureSense(*AIHearingConfiguration);
    AIPeripheralPerception->ConfigureSense(*AIPeripheralSightConfiguration);

    /* Reasonable defaults. */
    bEnableDebugMessages = false;
    TeamID = 1;
}

void AAIInfiltrationController::GetStates_Implementation(TArray<FString>& States)
{
	States = CurrentStates;
}

void AAIInfiltrationController::UpdateState_Implementation(EInfiltrationState NewState)
{
    /* Dummy implementation */
}

bool AAIInfiltrationController::QueryState(AActor * Target, FString QueryString)
{
	if (Target == nullptr) return false;

	AAIInfiltrationController* InfiltrationAIController = UInfiltrationUtilityLibrary::GetInfiltrationAIController(Target);
	if (InfiltrationAIController == nullptr) return false;

	IInfiltrationStateInterface* I = Cast<IInfiltrationStateInterface>(InfiltrationAIController);
	if (I != nullptr)
	{
		TArray<FString> TargetStates;
		I->Execute_GetStates(Target, TargetStates);

		if (TargetStates.Num() == 0) return false;
		else return TargetStates.Contains(QueryString);
	}
	else return false;
}

void AAIInfiltrationController::ReactToFriendly_Implementation(const TArray<FAIStimulus>& AIStimuli, AActor * PerceivedFriendly, UAIPerceptionComponent * SensingComponent)
{
    /* Dummy implementation. */
}

void AAIInfiltrationController::ReactToNeutral_Implementation(const TArray<FAIStimulus>& AIStimuli, AActor * PerceivedNeutral, UAIPerceptionComponent * SensingComponent)
{
    /* Dummy implementation. */
}

void AAIInfiltrationController::ReactToHostile_Implementation(const TArray<FAIStimulus>& AIStimuli, AActor * PerceivedHostile, UAIPerceptionComponent * SensingComponent)
{
    /* Dummy implementation. */
}

ETeamAttitude::Type AAIInfiltrationController::GetTeamAttitudeTowards(const AActor & Other) const
{
    const APawn* OtherPawn = Cast<APawn>(&Other);
    if (OtherPawn != nullptr)
    {
        const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
        if (TeamAgent != nullptr)
        {
            uint8 AgentTeamID = TeamAgent->GetGenericTeamId().GetId();
            if (AgentTeamID == TeamID) return ETeamAttitude::Type::Friendly;
            else if (NeutralTeamIDs.Contains(AgentTeamID)) return ETeamAttitude::Type::Neutral;
            else return ETeamAttitude::Type::Hostile;
        }
    }

    return ETeamAttitude::Type::Neutral;
}

void AAIInfiltrationController::OnPossess(APawn * ControlledPawn)
{
    Super::OnPossess(ControlledPawn);
    if (BehaviorTreeAsset != nullptr)
    {
        /* Initialize the blackboard from the Behavior Tree asset. If successful, start running the tree. Otherwise,
        print an error (this most likely indicates the behavior tree has no associated blackboard asset) */
        bool bSuccess = BlackboardComponent->InitializeBlackboard(*(BehaviorTreeAsset->BlackboardAsset));
        if (bSuccess)
        {
            /* Set values on the blackboard if flagged to do so. */
            if (bSetValuesOnPossess)
            {
               UInfiltrationUtilityLibrary::SetBoolValuesOnControllerBB(this, BoolValues);
               UInfiltrationUtilityLibrary::SetFloatValuesOnControllerBB(this, FloatValues);
               UInfiltrationUtilityLibrary::SetVectorValuesOnControllerBB(this, VectorValues);
               UInfiltrationUtilityLibrary::SetStringValuesOnControllerBB(this, StringValues);
            }

            SetGenericTeamId(FGenericTeamId(TeamID));
            AIPerception->OnPerceptionUpdated.AddDynamic(this, &AAIInfiltrationController::OnAIPerceptionUpdated);
            AIPeripheralPerception->OnPerceptionUpdated.AddDynamic(this, &AAIInfiltrationController::OnAIPeripheralPerceptionUpdated);
            BehaviorTreeComponent->StartTree(*BehaviorTreeAsset);
        }
        else
        {
            if (GEngine != nullptr && bEnableDebugMessages) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, 
            FString::Printf(TEXT("%s : Failed to initialize blackboard!"), *GetDebugName(this)));
        }
    }
    else
    {
        /* If the behavior tree asset has not been set, this likely indicates an error. */
        if (GEngine != nullptr && bEnableDebugMessages) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, 
        FString::Printf(TEXT("%s : When possessing, Behavior Tree Asset is null!"), *GetDebugName(this)));
    }
}

void AAIInfiltrationController::OnAIPerceptionUpdated(const TArray<AActor *>& DetectedActors)
{
    for (auto DetectedActor : DetectedActors)
    {
        FActorPerceptionBlueprintInfo DetectedInfo;
        bool bSuccess = AIPerception->GetActorsPerception(DetectedActor, DetectedInfo);

        if (bSuccess)
		{
			ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*DetectedInfo.Target);
			switch (Attitude)
			{
			case ETeamAttitude::Type::Hostile:
				ReactToHostile(DetectedInfo.LastSensedStimuli, DetectedActor, AIPerception);
				break;

			case ETeamAttitude::Type::Friendly:
				ReactToFriendly(DetectedInfo.LastSensedStimuli, DetectedActor, AIPerception);
				break;

            case ETeamAttitude::Type::Neutral:
				ReactToNeutral(DetectedInfo.LastSensedStimuli, DetectedActor, AIPerception);
				break;
			}
		}
    }
}

void AAIInfiltrationController::OnAIPeripheralPerceptionUpdated(const TArray<AActor *>& DetectedActors)
{
    for (auto DetectedActor : DetectedActors)
    {
        FActorPerceptionBlueprintInfo DetectedInfo;
        bool bSuccess = AIPeripheralPerception->GetActorsPerception(DetectedActor, DetectedInfo);

        if (bSuccess)
		{
			ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*DetectedInfo.Target);
			switch (Attitude)
			{
			case ETeamAttitude::Type::Hostile:
				ReactToHostile(DetectedInfo.LastSensedStimuli, DetectedActor, AIPeripheralPerception);
				break;

			case ETeamAttitude::Type::Friendly:
				ReactToFriendly(DetectedInfo.LastSensedStimuli, DetectedActor, AIPeripheralPerception);
				break;

            case ETeamAttitude::Type::Neutral:
				ReactToNeutral(DetectedInfo.LastSensedStimuli, DetectedActor, AIPeripheralPerception);
				break;
			}
		}
    }
}

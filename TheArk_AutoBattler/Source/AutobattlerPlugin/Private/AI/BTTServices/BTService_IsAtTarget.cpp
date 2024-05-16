// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTServices/BTService_IsAtTarget.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Types/AutobattlerEnums.h"
#include "Utility/AutobattlerFunctionLibrary.h"

/* Engine includes. */
#include "BehaviorTree/BlackboardComponent.h"

UBTService_IsAtTarget::UBTService_IsAtTarget()
{
    TargetingModeKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsAtTarget, TargetingModeKey), StaticEnum<ESkillTargetingMode>());
    TargetKeyActor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsAtTarget, TargetKeyActor), AActor::StaticClass());
	TargetKeyLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsAtTarget, TargetKeyLocation));
	RadiusKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsAtTarget, RadiusKey));
	IsAtGoalKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_IsAtTarget, IsAtGoalKey));
}

void UBTService_IsAtTarget::TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds)
{
    if (IsValid(ControlledCharacter) && IsValid(OwnerController))
    {
        if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
        {
            ESkillTargetingMode TargetingMode = (ESkillTargetingMode)BlackboardComp->GetValueAsEnum(TargetingModeKey.SelectedKeyName);
            if (TargetingMode == ESkillTargetingMode::Actor)
            {
                AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKeyActor.SelectedKeyName));
                if (!IsValid(TargetActor))
                {
                    OwnerController->ResetMovementState();
                    return;
                }

                // May be restored in the future is resetting movement state is necessary when switching characters.
                /*
                if (TargetActor != PreviousTargetingProperties.TargetCharacter)
                {
                    StopMovement();
                    IsMovingToTarget = false;
                }
                */

                if (TargetActor != ControlledCharacter && TrySetFocus) OwnerController->SetFocus(TargetActor);
		        float CartesianDistance = UAutobattlerFunctionLibrary::CartesianDistance(ControlledCharacter->GetActorLocation(), TargetActor->GetActorLocation());
		        BlackboardComp->SetValueAsBool(IsAtGoalKey.SelectedKeyName, CartesianDistance < BlackboardComp->GetValueAsFloat(RadiusKey.SelectedKeyName));
            }
            else if (TargetingMode == ESkillTargetingMode::Location)
            {
                FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetKeyLocation.SelectedKeyName);

                // May be restored in the future is resetting movement state is necessary when switching characters.
                /*
                if (!TargetLocation.Equals(PreviousTargetingProperties.TargetLocation, 1.0f))
                {
                    StopMovement();
                    IsMovingToTarget = false;
                }
                */

                if (TrySetFocus) OwnerController->SetFocalPoint(TargetLocation);
		        float CartesianDistance = UAutobattlerFunctionLibrary::CartesianDistance(ControlledCharacter->GetActorLocation(), TargetLocation);
		        BlackboardComp->SetValueAsBool(IsAtGoalKey.SelectedKeyName, CartesianDistance < BlackboardComp->GetValueAsFloat(RadiusKey.SelectedKeyName));
            }
            else if (TargetingMode == ESkillTargetingMode::None)
            {
                OwnerController->ResetMovementState();
            }
        }
    }
}

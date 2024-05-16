// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTServices/BTService_UpdateTargetingProps.h"

/* Autobattler includes. */
#include "AI/AutobattlerAIController.h"
#include "Game/Units/AutobattlerCharacter.h"
#include "Types/AutobattlerStructs.h"

/* Engine includes. */
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateTargetingProps::UBTService_UpdateTargetingProps()
{
    NodeName = "Update Targeting Properties";

    CurrentTargetingModeKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, CurrentTargetingModeKey), StaticEnum<ESkillTargetingMode>());
    PreviousTargetingModeKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, PreviousTargetingModeKey), StaticEnum<ESkillTargetingMode>());
    CurrentRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, CurrentRangeKey));
    PreviousRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, PreviousRangeKey));
    CurrentActorTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, CurrentActorTargetKey), AActor::StaticClass());
    PreviousActorTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, PreviousActorTargetKey), AActor::StaticClass());
    CurrentLocationTargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, CurrentLocationTargetKey));
    PreviousLocationTargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateTargetingProps, PreviousLocationTargetKey));
}

void UBTService_UpdateTargetingProps::TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!IsValid(OwnerController) || !IsValid(BlackboardComp)) return;

    FAbilityTargetingProperties PreviousTargetingProps;
    OwnerController->GetPreviousTargetingProperties(PreviousTargetingProps);

    BlackboardComp->SetValueAsEnum(PreviousTargetingModeKey.SelectedKeyName, (uint8)PreviousTargetingProps.TargetingMode);
    BlackboardComp->SetValueAsFloat(PreviousRangeKey.SelectedKeyName, PreviousTargetingProps.Range);
    BlackboardComp->SetValueAsObject(PreviousActorTargetKey.SelectedKeyName, PreviousTargetingProps.TargetCharacter);
    BlackboardComp->SetValueAsVector(PreviousLocationTargetKey.SelectedKeyName, PreviousTargetingProps.TargetLocation);

    EAbilityType RelevantAbilityType = OwnerController->GetIsAbilityQueued() || OwnerController->GetUsesAbilityOnly() ? EAbilityType::Skill : EAbilityType::Attack;
    const UAutobattlerSkill* RelevantSkill = OwnerController->GetRelevantSkill(RelevantAbilityType);
    FAbilityTargetingProperties TargetingProperties = OwnerController->GetAbilityTargetingProperties(RelevantSkill);

    BlackboardComp->SetValueAsEnum(CurrentTargetingModeKey.SelectedKeyName, (uint8)TargetingProperties.TargetingMode);
    BlackboardComp->SetValueAsFloat(CurrentRangeKey.SelectedKeyName, TargetingProperties.Range);
    BlackboardComp->SetValueAsObject(CurrentActorTargetKey.SelectedKeyName, TargetingProperties.TargetCharacter);
    BlackboardComp->SetValueAsVector(CurrentLocationTargetKey.SelectedKeyName, TargetingProperties.TargetLocation);

    OwnerController->SetPreviousTargetingProperties(TargetingProperties);
    OwnerController->SetLastTarget(TargetingProperties.TargetCharacter);
}

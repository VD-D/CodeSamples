// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

/* Class header. */
#include "AI/BTTServices/BTService_HasStraightPath.h"

/* Autobattler includes. */
#include "Game/Units/AutobattlerCharacter.h"

/* Engine includes. */
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTService_HasStraightPath::UBTService_HasStraightPath()
{
    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_HasStraightPath, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_HasStraightPath, TargetKey));
	HasStraightPathKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_HasStraightPath, HasStraightPathKey));

    SphereTraceRadius = 34.0f;
    TraceType = ETraceType::Profile;
    TraceChannel = ECollisionChannel::ECC_Visibility;
    ProfileName = FName("Pawn");
}

void UBTService_HasStraightPath::TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!IsValid(BlackboardComp) || !IsValid(ControlledCharacter) || !IsValid(GetWorld())) return;

    FHitResult Hit;
    bool HitOccurred = true;

    AActor* TargetActor = nullptr; 
    FVector TargetLocation = FVector::ZeroVector;

    if (TargetKey.SelectedKeyType.Get() == UBlackboardKeyType_Object::StaticClass()) TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
    else if (TargetKey.SelectedKeyType.Get() == UBlackboardKeyType_Vector::StaticClass()) TargetLocation = BlackboardComp->GetValueAsVector(TargetKey.SelectedKeyName);

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(ControlledCharacter);
    if (IsValid(TargetActor)) 
    {
        QueryParams.AddIgnoredActor(TargetActor);
        TargetLocation = TargetActor->GetActorLocation();
    }

    if (TraceType == ETraceType::Channel)
    {
        HitOccurred = GetWorld()->SweepSingleByChannel(
            Hit,
            ControlledCharacter->GetActorLocation(),
            TargetLocation,
            FQuat::Identity,
            TraceChannel,
            FCollisionShape::MakeSphere(FMath::Max(1.0f, FMath::Abs(SphereTraceRadius))),
            QueryParams
        );
    }
    else if (TraceType == ETraceType::ObjectType)
    {
        FCollisionObjectQueryParams ObjectQueryParams;
        for (auto CollisionType : ObjectChannels) ObjectQueryParams.AddObjectTypesToQuery(CollisionType);

        HitOccurred = GetWorld()->SweepSingleByObjectType(
            Hit,
            ControlledCharacter->GetActorLocation(),
            TargetLocation,
            FQuat::Identity,
            ObjectQueryParams,
            FCollisionShape::MakeSphere(FMath::Max(1.0f, FMath::Abs(SphereTraceRadius))),
            QueryParams
        );
    }
    else if (TraceType == ETraceType::Profile)
    {
        HitOccurred = GetWorld()->SweepSingleByProfile(
            Hit,
            ControlledCharacter->GetActorLocation(),
            TargetLocation,
            FQuat::Identity,
            ProfileName,
            FCollisionShape::MakeSphere(FMath::Max(1.0f, FMath::Abs(SphereTraceRadius))),
            QueryParams
        );
    }

    BlackboardComp->SetValueAsBool(HasStraightPathKey.SelectedKeyName, !HitOccurred);
}

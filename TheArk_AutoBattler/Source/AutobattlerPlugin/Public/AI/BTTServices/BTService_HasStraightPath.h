// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTServices/BTService_Autobattler.h"
#include "BTService_HasStraightPath.generated.h"

/* Used to configure sphere trace. */
UENUM(BlueprintType)
enum class ETraceType : uint8
{
	Channel UMETA(DisplayName = "Channel"),
	ObjectType UMETA(DisplayName = "ObjectType"),
	Profile UMETA(DisplayName = "Profile"),
	Count UMETA(Hidden)
};

/**
 * Service which checks if the controlled pawn has a direct (straight) path to the target.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UBTService_HasStraightPath : public UBTService_Autobattler
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* (IN) The target we want to check path to. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKey;

	/* (OUT) Whether we have a straight path or not. */
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector HasStraightPathKey;

	/* Radius of trace. */
	UPROPERTY(EditAnywhere, Category = "Has Straight Path")
	float SphereTraceRadius;

	/* What type of trace to use. */
	UPROPERTY(EditAnywhere, Category = "Has Straight Path")
	ETraceType TraceType;

	/* Trace collision channel. */
	UPROPERTY(EditAnywhere, Category = "Has Straight Path", meta = (EditCondition = "TraceType==ETraceType::Channel"))
	TEnumAsByte<ECollisionChannel> TraceChannel;

	/* Trace object types. */
	UPROPERTY(EditAnywhere, Category = "Has Straight Path", meta = (EditCondition = "TraceType==ETraceType::ObjectType"))
	TArray<TEnumAsByte<ECollisionChannel>> ObjectChannels;

	/* Trace profile name. */
	UPROPERTY(EditAnywhere, Category = "Has Straight Path", meta = (EditCondition = "TraceType==ETraceType::Profile"))
	FName ProfileName;

/////////////////////////////////////////////////////////////////////////////////
//// BT SERVICE INTERFACE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Constructor.
	 */
	UBTService_HasStraightPath();

	/**
	 * Checks if Controlled Character has straight path to target.
	 * @param OwnerController Unused.
	 * @param ControlledCharacter Checked if within range of character.
	 * @param DeltaSeconds Unused.
	 */
	virtual void TickAutobattlerNode(UBehaviorTreeComponent& OwnerComp, AAutobattlerAIController* OwnerController, AAutobattlerCharacter* ControlledCharacter, float DeltaSeconds) override;
};

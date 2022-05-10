 // Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FixedCameraDelegator.generated.h"

UCLASS()
class SACRUSMORDUM_API AFixedCameraDelegator : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- CONFIGURABLE --- */

	/* Map which should contain the fixed cameras in the scene. 
	Key = "sequence" in which the camera appears. 
	Value = pointer to the camera itself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera Delegator|Configurable")
	TMap<int32, class AFixedCamera*> CamerasMap;

	/* Name of this camera delegator. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera Delegator|Configurable")
	FName CameraDelegatorName;

	/* Key from CamerasMap at which to start. If this key does not exist, it will start at the first pair found in the map. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera Delegator|Configurable", meta = (ClampMin = "0"))
	int32 StartKey;

	/* Whether or not to auto-possess on begin play (may dispossess other pawns). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera Delegator|Configurable")
	bool bAutoPossessOnBeginPlay;

	/* --- COMPONENTS --- */

	/* Icon to make this actor visible in the editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fixed Camera Delegator Components")
	class UBillboardComponent* Billboard;
		
/* --- FUNCTIONS --- */
public:	
	/** Constructor.
	  */
	AFixedCameraDelegator();

	/* --- BLUEPRINT EXPOSED --- */

	/** Attempts to switch to another camera.
	  * KeyIndex - The value equivalent to the key in CamerasMap. Function does nothing if this value is invalid.
	  */
	UFUNCTION(BlueprintCallable, Category = "Fixed Camera Delegator")
	void SwitchCamera(int32 KeyIndex);

protected:
	/** Will automatically switch to first camera at StartKey if bAutoPossessOnBeginPlay = true.
	  */
	virtual void BeginPlay() override;
};

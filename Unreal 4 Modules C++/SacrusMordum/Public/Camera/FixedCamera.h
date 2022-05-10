// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FixedCamera.generated.h"

UCLASS()
class SACRUSMORDUM_API AFixedCamera : public APawn
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- CAMERA ROTATION VALUES --- */

	/* Max pitch rotation of the camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera|Configurable", meta = (ClampMin = "-180", ClampMax = "180"))
	float PitchRotationMax;

	/* Min pitch rotation of the camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera|Configurable", meta = (ClampMin = "-180", ClampMax = "180"))
	float PitchRotationMin;

	/* Max yaw rotation of the camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera|Configurable", meta = (ClampMin = "-180", ClampMax = "180"))
	float YawRotationMax;

	/* Min yaw rotation of the camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera|Configurable", meta = (ClampMin = "-180", ClampMax = "180"))
	float YawRotationMin;

	/* Next camera index, only does anything if > 0. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Fixed Camera|Configurable", meta = (ClampMin = "-1"))
	int32 NextCameraIndex;

	/* Previous camera index, only does anything if > 0. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Fixed Camera|Configurable", meta = (ClampMin = "-1"))
	int32 PrevCameraIndex;

	/* Pointer to FixedCameraDelegator in scene. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Fixed Camera|Configurable")
	class AFixedCameraDelegator* FixedCameraDelegator;

	/* --- COMPONENTS --- */

	/* Scene root, to ensure that camera rotation limits work properly. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fixed Camera Components")
	class USceneComponent* SceneRoot;

	/* The actual camera. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fixed Camera Components")
	class UCameraComponent* Camera;

/* --- FUNCTIONS --- */
public:
	/** Constructor.
	  */
	AFixedCamera();

	/** Tries to move to the next camera in the CamerasMap found in the FixedCameraDelegator.
	  */
	UFUNCTION(BlueprintCallable, Category = "Fixed Camera")
	void NextCamera();

	/** Tries to move to the previous camera in the CamerasMap found in the FixedCameraDelegator.
	  */
	UFUNCTION(BlueprintCallable, Category = "Fixed Camera")
	void PreviousCamera();

	/** Native override of PossessedBy, normally to set Pitch and Yaw Rotation limits on the player camera manager.
	  * @param NewController - The controller which possessed this pawn.
	  */
	virtual void PossessedBy(class AController* NewController) override;

protected:
	/** Dummy begin play.
	  */
	virtual void BeginPlay() override;
};

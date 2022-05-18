// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "InGameInspectable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInspectionBegan, class UInspectableWidget*, InspectionWidget);

USTRUCT(BlueprintType)
struct FInspectableProperties : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Length of target capture spring arm length. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game Inspectable")
	float TargetCaptureArmLength;

	/* When spawning, this is initial relative transform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game Inspectable")
	FTransform TargetCaptureRelativeTransform;

	/* Optional text when inspecting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game Inspectable")
	FText InspectableText;

	/* Default constructor. */
	FInspectableProperties()
	{
		TargetCaptureArmLength = 50.0f;
		TargetCaptureRelativeTransform = FTransform::Identity;
		InspectableText = FText::GetEmpty();
	}
};

UCLASS()
class INSPECTABLES_API AInGameInspectable : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "In Game Inspectable")
	class AInspectableCapture* TargetCapture;

	/* If TargetCapture is null, on begin play, try to find a valid one. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game Inspectable")
	bool bTryAutoFindTargetCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game Inspectable", meta=(RowType="InspectableProperties"))
	class UDataTable* DefaultDatatable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game Inspectable")
	FName DefaultRowName;

	UPROPERTY(BlueprintAssignable, Category = "In Game Inspectable")
	FInspectionBegan InspectionBeganDelegate;

private:
	class AInGameInspectable* ReplicatedInspectable;
	
/* --- FUNCTIONS --- */
public:
	/**
	 * Constructor.
	 */
	AInGameInspectable();

	/**
	 * If TargetCapture is valid, creates a clone of this actor at the TargetCapture's location
	 * relatively transformed by TargetCaptureRelativeTransform. This allows it to be fed to a render
	 * target, in turned used by a widget.
	 * NOTE: Since we are creating a clone, any BeginPlay functions will trigger.
	 * @param InspectableProperties Properties to apply to inspection.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game Inspectable")
	void BeginInspection(const FInspectableProperties& InspectableProperties);

	/**
	 * Same as BeginInspection, except it passes row from CustomDatatable.
	 * @param CustomDatatable Datatable to read properties from
	 * @param CustomDatatableRowName Row name in CustomDatatable.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game Inspectable")
	void BeginInspectionCustomDatatable(class UDataTable* CustomDatatable, const FName& CustomDatatableRowName);

	/**
	 * Same as BeginInspection, except it passes row from DefaultDatatable using DefaultRowName.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game Inspectable")
	void BeginInspectionDefaultDatatable();

	/**
	 * If inspecting an item, add local roll rotation.
	 * @param DeltaRoll Roll to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game Inspectable")
	void InspectableAddRoll(float DeltaRoll);

	/**
	 * If inspecting an item, add local pitch rotation.
	 * @param DeltaPitch Pitch to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game Inspectable")
	void InspectableAddPitch(float DeltaPitch);

	/**
	 * If inspecting an item, add local yaw rotation.
	 * @param DeltaYaw Yaw to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game Inspectable")
	void InspectableAddYaw(float DeltaYaw);

protected:
	/**
	 * Begin play to try and auto find target capture.
	 */
	virtual void BeginPlay() override;
};

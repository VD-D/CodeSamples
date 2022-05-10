// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/FocusInterface.h"
#include "FocusWidget.generated.h"

/* Class which provides a "focus percentage" (value from 0 to 1) to fill out a progress bar. */
UCLASS()
class SACRUSMORDUM_API UFocusWidget : public UUserWidget, public IFocusInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Focus Widget")
	float FocusPercentage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Focus Widget")
	ESlateVisibility DesiredVisibility;

/* --- FUNCTIONS --- */
public:
	/** Updates the percentage, but clamps the value between 0 to 1.
	  * @param NewPercentage - The FocusPercentage to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Focus Widget", meta = (DisplayName = "Update Percentage"))
	void UpdatePercentage(float NewPercentage);

	/** Updates desired component visibility.
	  * @param NewVisibility - The Visibility to set
	  */
	UFUNCTION(BlueprintCallable, Category = "Focus Widget", meta = (DisplayName = "Update Slate Visibility"))
	void UpdateFocusWidgetComponentVisibility(ESlateVisibility NewVisibility);

protected:
	/* --- FOCUS INTERFACE --- */

	/** Dummy declaration for on focus begun.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnFocusBegan();
	virtual void OnFocusBegan_Implementation() override;

	/** Dummy declaration for on focus interrupted.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnFocusInterrupted();
	virtual void OnFocusInterrupted_Implementation() override;

	/** Dummy declaration for on focus completed.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnFocusCompleted();
	virtual void OnFocusCompleted_Implementation() override;
	
};

// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/FocusInterface.h"
#include "Interfaces/InvestigationDiscoveryInterface.h"
#include "SharedStructures/InvestigationInfoProperties.h"
#include "InvestigationInfoWidget.generated.h"

/* Widget which allows a display of an InvestigationObjects info to the player. */
UCLASS()
class SACRUSMORDUM_API UInvestigationInfoWidget : public UUserWidget, public IFocusInterface, public IInvestigationDiscoveryInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
protected:
	/* --- DEFAULT IMAGE PROPERTIES --- */

	/* X size of the image. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Info Widget")
	float ImageXSize;

	/* Y size of the image. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Info Widget")
	float ImageYSize;

	/* --- DESINGER EXPOSED --- */

	/* The formatted name of the investigation object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Info Widget")
	FText ObjectName;

	/* The formatted information about the object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Info Widget")
	TArray<FText> ObjectInformation;

	/* The slate brush representing the image. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Info Widget")
	FSlateBrush ImageInfo;

/* --- FUNCTIONS --- */
public:
	/** Updates this widgets investigation info.
	  * @param NewInvestigationObjectProperties - New info to set.
	  */
	UFUNCTION(BlueprintCallable, Category = "Investigation Info Widget", meta = (DisplayName = "Update Investigation Widget Info"))
	void UpdateInvestigationInfo(FInvestigationObjectProperties NewInvestigationObjectProperties);

	/** Indicates that transition animation should begin. This exists to be overridden by a class which actually implements the animations.
	  * @param Direction - Forward if true, backward if false.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Investigation Info Widget", meta = (DisplayName = "Should Enter Transition Animation"))
	void ShouldEnterTransitionAnimation(bool Direction);
	virtual void ShouldEnterTransitionAnimation_Implementation(bool Direction);

	/** Updates the scroll bar offset. This should be overriden by a class which contains a scroll bar.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Investigation Info Widget", meta = (DisplayName = "Update Scroll Bar Offset"))
	void UpdateScrollBarOffset(float AxisValue);
	virtual void UpdateScrollBarOffset_Implementation(float AxisValue);

protected:
	/* --- TEXT UPDATED --- */

	/** Before any text is added by UpdateInvestigationInfo, this function is called, allowing for any pre-adding
	  * functionality to be implemented.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Text Updates")
	void PreInformationUpdated();
	virtual void PreInformationUpdated_Implementation();

	/** Whenever text is added from UpdateInvestigationInfo, this function is called, along with its respective tags.
	  * @param NewInformation - The text content.
	  * @param Tags - The tags associated with the text.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Text Updates")
	void OnInformationUpdated(const FText& NewInformation, const TArray<FName>& Tags);
	virtual	void OnInformationUpdated_Implementation(const FText& NewInformation, const TArray<FName>& Tags);

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

	/* --- INVESTIGATION DISCOVERY INTERFACE --- */

	/** Declaration of investigation object found.
	  * @param FoundObject - The found investigation object.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnInvestigationObjectFound(class AInvestigationObject* FoundObject);
	virtual void OnInvestigationObjectFound_Implementation(class AInvestigationObject* FoundObject) override;

	/** Declaration of investigation object lost.
	  * @param FoundObject - The lost investigation object.
	  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Focus Interface")
	void OnInvestigationObjectLost(class AInvestigationObject* LostObject);
	virtual void OnInvestigationObjectLost_Implementation(class AInvestigationObject* LostObject) override;

	/* --- UTILITY --- */

	/** Utility function which computes the appropriate value to update a scroll bar with.
	  * @param ScrollBar - The scroll bar whose scroll offset will be updated.
	  * @param AxisValue - The axis value, typically from the mouse wheel.
	  * @param SpeedMultiplier - How "fast" the scroll bar scrolls. Set to be 0.1 if less than 0.1.
	  * @param UseInterpolation - Whether or not to interpolate or "smooth" scroll bar scrolling.
	  * @param InterpolationSpeed - Speed at which interpolation happens, only relevant if UseInterpolation = true.
	  */
	UFUNCTION(BlueprintCallable, Category = "Investigation Info Utility")
	void UpdateScrollBar(class UScrollBox* ScrollBar, float AxisValue, float SpeedMultiplier = 10.0f, bool UseInterpolation = true, float InterpolationSpeed = 3.0f);

private:
	/** Utility function for for formatting an FText based off an FString and a TArray<FString>.
	  * E.g. Base = "My name is: {0} and my age is {1}." FormatArray = {Bob, 20} returns "My name is: Bob and my age is 20."
	  * @param Base - The base string to be formatted.
	  * @param FormatArray - Variable number of arguments to format.
	  */
	FText VariableFormatString(FString Base, const TArray<FString>& FormatArray);
	
};

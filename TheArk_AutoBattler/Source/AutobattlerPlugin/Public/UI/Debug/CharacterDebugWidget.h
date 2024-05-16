// Copyright Juggler Games 2024
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerEnums.h"
#include "CharacterDebugWidget.generated.h"

class AAutobattlerAIController;
class UCheckBox;
class UTextBlock;

/**
 * Widget which visualises debug information about a character.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UCharacterDebugWidget : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Visualises ID of controlled character. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* IDText;

	/* Visualises owner type of controlled character. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* OwnerText;

	/* Visualises current action. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentActionText;

	/* Visualises current targeting type. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentTargetingTypeText;

	/* Checkbox for enabling/disabling target trace drawing. */
	UPROPERTY(meta = (BindWidget))
	UCheckBox* VisualiseTargetCheckbox;

	/* Checkbox for enabling/disabling move to location trace drawing. */
	UPROPERTY(meta = (BindWidget))
	UCheckBox* VisualiseMoveToLocationCheckbox;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Maps action type enum to user friendly name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DebugWidget")
	TMap<EActionType, FText> ActionToTextMap;

	/* Maps targeting mode to user friendly name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DebugWidget")
	TMap<ESkillTargetingMode, FText> TargetingToTextMap;

	/* Maps owner type to user friendly name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DebugWidget")
	TMap<EEntity, FText> OwnerToTextMap;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Controller of currnet debug character. */
	UPROPERTY()
	AAutobattlerAIController* CurrentDebugController;

	/* Enables visualising what location/character is the target of debug character. */
	bool VisualiseCurrentTarget;

	/* Visualises where the debug character wants to move to. */
	bool VisualiseMoveToPoint;

	/* Whether we should update information on the debug widget. */
	bool EnableUpdate;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Binds delegates.
	 */
	virtual void NativeConstruct() override;

/////////////////////////////////////////////////////////////////////////////////
//// UPDATE
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Updates information on the widget.
	 * @param MyGemometry Unused.
	 * @param InDeltaTime Unused.
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/**
	 * Draws a debug line from debug controlled character to their target.
	 */
	void DrawLineToTarget();

	/**
	 * Draws a debug line from debug controlled character to their desired move to point.
	 */
	void DrawLineToMoveToPoint();

	/**
	 * Updates widgets displaying text with most recent parameters.
	 */
	void UpdateTextWidgets();

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Sets a new controller for this debug widget to use.
	 * @param NewController New controller to use.
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug Widget")
	void SetNewDebugController(AAutobattlerAIController* NewController);

/////////////////////////////////////////////////////////////////////////////////
//// DELEGATE
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Updates whether we should visualise drawing debug traces to target.
	 * @param NewState Whether we should visualise drawing debug traces to target.
	 */
	UFUNCTION()
	void OnVisualiseTargetCheckboxStateChange(bool NewState);

	/**
	 * Updates whether we should visualise drawing debug traces to move to location.
	 * @param NewState Whether we should visualise drawing debug traces to move to location.
	 */
	UFUNCTION()
	void OnVisualiseMoveToLocationCheckboxStateChange(bool NewState);
};

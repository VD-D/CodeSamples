// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AutobattlerEnums.h"
#include "AutobattlerHUD.generated.h"

class UButton;
class UCharacterSelectContainerWidget;
class UPlayerAbilityContainer;
class UTextBlock;

/**
 * Widget which serves as a container for other persistent widgets relevant to the autobattler.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerHUD : public UUserWidget
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// WIDGET BINDINGS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Roster of characters which can be deployed. */
	UPROPERTY(meta = (BindWidget))
	UCharacterSelectContainerWidget* CharacterSelectContainer;

	/* Container with player abilities in it. */
	UPROPERTY(meta = (BindWidget))
	UPlayerAbilityContainer* PlayerAbilityContainer;

	/* Shows/hides currently available deployment fields. */
	UPROPERTY(meta = (BindWidget))
	UButton* ToggleAvailableDeploymentFields;

	/* Currently used up budget. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentBudget;

	/* Budget about to be used, e.g. by floating character. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PendingBudget;

	/* Max budget. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxBudget;

	/* What the current phase of the game is. */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGamePhase;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Maps an autobattler phase to a user-friendly display name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autobattler UI")
	TMap<EAutobattlerPhase, FText> PhaseToTextMap;

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Getter for Character Select Container widget.
	 * @return Character Select Container widget.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler UI")
	UCharacterSelectContainerWidget* GetCharacterSelectContainer() const { return CharacterSelectContainer; }

	/**
	 * Getter for Player Ability Container widget.
	 * @return Player Ability Container widget.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler UI")
	UPlayerAbilityContainer* GetPlayerAbilityContainer() const { return PlayerAbilityContainer; }

	/**
	 * Sets value of current budget text.
	 * @param NewCurrentBudget Value to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetCurrentBudget(int32 NewCurrentBudget);

	/**
	 * Sets value of max budget text.
	 * @param NewMaxBudget Value to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetMaxBudget(int32 NewMaxBudget);

	/**
	 * Set value of pending budget text.
	 * @param NewPendingBudget Value to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetPendingBudget(int32 NewPendingBudget);

	/**
	 * Whether pending budget text should be visible.
	 * @param NewPendingBudget Value to set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetPendingBudgetVisibility(bool ShouldBeVisible);

	/**
	 * Sets the game phase text.
	 * @param PhaseToSet Phase for which to set text for.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetAutobattlerGamePhaseText(EAutobattlerPhase PhaseToSet);

	/**
	 * Enables/disables deployment (selecting characters from UI to deploy).
	 * @param IsEnabled True enables deployment, false disables it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler UI")
	void SetDeploymentEnabled(bool IsEnabled);

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * If BP class wishes to further alter appearance after placement reason is changed.
	 * @param PlacementReason New placement status.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler UI")
	void OnPlacementReasonChange(EPlaceReason PlacementReason);
	virtual void OnPlacementReasonChange_Implementation(EPlaceReason PlacementReason);

protected:
	/**
	 * Binds delegates.
	 */
	virtual void NativeConstruct() override;

	/**
	 * If BP class wishes to further alter appearance after pending budget visibility is changed.
	 * @param NewShouldBeVisible Whether we want pending budget text to be visible.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler UI")
	void OnPendingBudgetVisibilityChanged(bool NewShouldBeVisible); 
	virtual void OnPendingBudgetVisibilityChanged_Implementation(bool NewShouldBeVisible);

private:
	/**
	 * Internal helper function for setting a text block's text to a given int value.
	 * @param TextBlock Text block to set value for.
	 * @param Value Value to set.
	 */
	void SetIntAsText(UTextBlock* TextBlock, int32 Value);

	/**
	 * Attempts to show/hide available deployment fields for the player.
	 */
	UFUNCTION()
	void OnToggleAvailableDeploymentFieldsClicked();
};

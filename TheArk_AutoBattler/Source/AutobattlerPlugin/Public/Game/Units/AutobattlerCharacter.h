// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/AutobattlerMouseInterface.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerCharacter.generated.h"

class UAutobattlerAnimInstance;
class UAutobattlerAbility;
class UAutobattlerAttack;
class UAutobattlerChargeComponent;
class UAutobattlerSkill;
class UBoxComponent;
class UCharacterPanelComponent;
class UChargeBar;
class UCustomDepthComponent;
class UDefaultAttack;
class UEnvQuery;
class UHealthBar;
class UNavModifierComponent;
class USkillImplementation;
class USphereComponent;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionChanged, EActionType, NextAction, AAutobattlerCharacter*, UpdatedCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateChanged);

UCLASS()
class AUTOBATTLERPLUGIN_API AAutobattlerCharacter : public ACharacter, public IAutobattlerMouseInterface
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* A bounding box which defines the space this character takes up during deployment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DeploymentBounds;

	/* Component which displays the health bar. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* HealthBarDisplay;

	/* Component which displays the charge bar. Only shown if the character actually uses
	charges. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* ChargeBarDisplay;

	/* Component which enables displaying the character panel. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacterPanelComponent* CharacterPanelComponent;

	/* Component used for rendering meshes in custom depth. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCustomDepthComponent* CustomDepthComponent;

/////////////////////////////////////////////////////////////////////////////////
//// DELEGATES
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Delegate broadcast when this character's action changes. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnActionChanged ActionChanged;

	/* Delegate broadcast when this character loses health. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnStateChanged OnHealthLost;

	/* Delegate broadcast when this character loses health. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnStateChanged OnHealthGained;

/////////////////////////////////////////////////////////////////////////////////
//// PRIVATE & PROTECTED VARIABLES
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Environmental query for picking a point to path around this character. */
	UPROPERTY()
	UEnvQuery* PathAroundQuery;

	/* Environmental query for picking a point surrounding this character. */
	UPROPERTY()
	UEnvQuery* SurroundingPointsQuery;

	/* The next action we wish to perform. */
	EActionType CurrentAction;

	/* The damage type this character deals. */
	EDamageType CharacterDamageType;

	/* The resistance type of this character. */
	EResistanceType CharacterResistanceType;

	/* Current game phase. */
	EAutobattlerPhase CurrentGamePhase;

	/* Health values for this character. */
	UPROPERTY(ReplicatedUsing="OnRep_CurrentHealth")
	float CurrentHealth;

	UPROPERTY(ReplicatedUsing="OnRep_MaxHealth")
	float MaxHealth;

	/* ID of identity owning this character, e.g. Player1, Player2, etc. */
	UPROPERTY(ReplicatedUsing="OnRep_OwnerID")
	EEntity OwnerID;

	/* Internal ID of this character. */
	UPROPERTY(Replicated)
	int32 ID;

	/* How expensive this character is with respect to budget. */
	UPROPERTY(ReplicatedUsing="OnRep_BudgetChanged")
	int32 BudgetCost;

	/* Charge value used by this character (if ability uses charges). */
	UPROPERTY(ReplicatedUsing="OnRep_GainCharges")
	int32 Charges;

	UPROPERTY(ReplicatedUsing="OnRep_MaxCharges")
	int32 MaxCharges;

	/* Modifiers */
	float AttackSpeedModifier;
	float SkillSpeedModifier;
	float AttackRangeModifier;
	float AbilityRangeModifier;

	/* Critical properties for this character. */
	float CriticalChance;
	float CriticalMultiplier;

	/* Resurrection properties for this character. */
	bool CanRessurect;
	bool HasRessurected;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default contructor.
	 */
	AAutobattlerCharacter();

	/**
	 * Generates a character from the current listing.
	 * @param WhoOwns Whether this character is owned by the server, client or is an enemy.
	 * @param CharacterListing Listing from which to generate this character from. 
	 */
	void BuildCharacterFromListing(EEntity WhoOwns, int32 CharacterID, const FCharacterListing& CharacterListing);

protected:
	/**
	 * Binds delegates.
	 */
	virtual void BeginPlay() override;

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Getter for deployment bounds of this character.
	 * @return Deployment bounds of this character.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	UBoxComponent* GetDeploymentBounds() const { return DeploymentBounds; }

	/**
	 * Getter for the character panel component this character.
	 * @return Character panel of this character.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	UCharacterPanelComponent* GetCharacterPanelComponent() const { return CharacterPanelComponent; }

	/**
	 * Returns the env query for pathing around this character.
	 * Valid only on server
	 * @return Env query for pathing around this character.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	UEnvQuery* GetPathAroundQuery() const { return PathAroundQuery; }

	/**
	 * Returns the env query for finding points around this character.
	 * Valid only on server
	 * @return Env query for finding points around this character.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	UEnvQuery* GetSurroundingPointsQuery() const { return SurroundingPointsQuery; }

	/**
	 * Getter for the damage type this character deals.
	 * @return Damage type this character deals. 
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EDamageType GetDamageType() const { return CharacterDamageType; }

	/**
	 * Getter for this character's resistance type.
	 * @return This character's resistance type.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EResistanceType GetResistanceType() const { return CharacterResistanceType; }

	/**
	 * Getter for this character's current action.
	 * @return This character's current action.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EActionType GetCurrentAction() const { return CurrentAction; }

	/**
	 * Getter for this character's internal ID.
	 * @return This character's internal ID.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	int32 GetID() const { return ID; }

	/**
	 * Getter for this character's budget cost.
	 * @return This character's budget cost.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	int32 GetBudgetCost() const { return BudgetCost; }

	/**
	 * Getter for this character's owner's identity.
	 * @return This character's owner's identity.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EEntity GetOwnerIdentity() const { return OwnerID; }

	/**
	 * Get whether or not this character is dead.
	 * @return Whether this character is dead.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetIsDead() const { return CurrentAction == EActionType::Dead; }

	/**
	 * Getter for whether or not this character uses a charge-based ability.
	 * @return Does use a charge-based ability. 
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetUsesCharges() const;

	/**
	 * Getter for critical chance.
	 * @return Critical chance. 
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	float GetCriticalChance() const { return CriticalChance; }

	/**
	 * Getter for critical multiplier.
	 * @return Critical multiplier.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	float GetCriticalMultiplier() const { return CriticalMultiplier; }

	/**
	 * Getter for max health.
	 * @return Max Health. 
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	float GetMaxHealth() const { return MaxHealth; }

	/**
	 * Getter for current health.
	 * @return Current Health. 
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	float GetCurrentHealth() const { return CurrentHealth; }

	/**
	 * Getter to modifier to range of ability useage (i.e. cast range).
	 * @return Modifier to  range of ability useage (i.e. cast range).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	float GetAbilityRangeModifier() const { return AbilityRangeModifier; }

	/**
	 * Getter to modifier to range of attacks (i.e. attack range).
	 * @return Modifier to range of attacks (i.e. attack range).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	float GetAttackRangeModifier() const { return AttackRangeModifier; }

	/**
	 * Sets whether this character will ressurect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void SetCanRessurect() { CanRessurect = true; }

	/**
	 * Sets health to the new value.
	 * @param NewHealth Health value to set. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void SetCurrentHealth(float NewHealth);

	/**
	 * Sets the next action type for this character.
	 * Note that you cannot directly set EActionType::Dead. Call Kill() instead.
	 * @param NewAction Action to set. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void SetActionType(EActionType NewAction);

/////////////////////////////////////////////////////////////////////////////////
//// GAME
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Kills this character. This happens automatically when CurrentHealth becomes 0, but
	 * characters can die through other means. 
	 */
	void Kill();

	/**
	 * Modifies a given stat.
	 * @param StatToModify Which stat should be modified.
	 * @param ModifierScale By how much the stat should be increased/decreased.
	 * @param IsPermanent Whether the stat permanently should be permanently modified.
	 * @param ExpiryDuration If not a permanent modification, how much time should elapse before the modifier wears off.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ModifyStatistic(EAutobattlerStatType StatToModify, float ModifierScale, bool IsPermanent, float ExpiryDuration);

	/**
	 * Modifies this character's resistance type.
	 * @param NewResistanceType New resistance type to modify to.
	 * @param IsPermanent Whether this is the character's new (permanent) resistance type or whether the change is temporary.
	 * @param ExpiryDuration If not a permanent modification, how much time should elapse before the resistance type changes back to normal.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ModifyResistanceType(EResistanceType NewResistanceType, bool IsPermanent, float ExpiryDuration);

	/**
	 * Modifies this character's damage type.
	 * @param NewResistanceType New damage type to modify to.
	 * @param IsPermanent Whether this is the character's new (permanent) damage type or whether the change is temporary.
	 * @param ExpiryDuration If not a permanent modification, how much time should elapse before the damage type changes back to normal.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ModifyDamageType(EDamageType NewDamageType, bool IsPermanent, float ExpiryDuration);

	/**
	 * Server-side only configuration of the component which will have the character gain charges.
	 * @param ChargeComponentClass Class of component
	 */
	void ConfigureGetChargeComponent(TSubclassOf<UAutobattlerChargeComponent> ChargeComponentClass, int32 ChargesToTriggerAbility);

	/**
	 * Gains a charge, which will only be relevant if ability actually uses charges.
	 * If we reach the charge cap, we will trigger the skill. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void GainCharge();

	/**
	 * This is only relevant for a character which has died but should be resurrecting. Revives the character
	 * if this is the case, otherwise does nothing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void EndRessurect();

	/**
	 * Locally toggles UI, e.g. if called on client it will only be hidden on client.
	 * @param ShouldBeVisible Whether UI elements should be visible locally.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ToggleUIElementsLocal(bool ShouldBeVisible);

private:
	/**
	 * Modifies a given stat.
	 * @param StatToModify Which stat should be modified.
	 * @param ModifierScale By how much the stat should be increased/decreased.
	 * @param SetTimer If true, a timer will be set "undoing" the effects of the stat after ExpiryDuration
	 * @param ExpiryDuration If not a permanent modification, how much time should elapse before the modifier wears off.
	 */
	UFUNCTION()
	void ModifyStatisticInternal(EAutobattlerStatType StatToModify, float ModifierScale);

	/**
	 * Updates UI on server and client to reflect a stat being modified.
	 * @param StatToModify Which stat should be modified.
	 * @param ModifierScale By how much the stat should be increased/decreased.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateCharacterPanelStat(EAutobattlerStatType StatToModify, float ModifierScale);

	/**
	 * Updates the resistance type on the UI for both server and client.
	 * @param IsPermanent Whether propagated changes are permanent.
	 */
	void UpdateResistanceUI(bool IsPermanent);

	/**
	 * Updates the damage type on the UI for both server and client.
	 * @param IsPermanent Whether propagated changes are permanent.
	 */
	void UpdateDamageTypeUI(bool IsPermanent);

	/**
	 * Internal function used to update a discrete stat on the character panel component.
	 * @param StatToModify Actual stat we are modifying.
	 * @param NewText New text to display.
	 * @param IsPermanent Whether or not the change is permanent.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void UpdateDiscreteStat(EAutobattlerDiscreteType StatToModify, const FText& NewText, bool IsPermanent);

/////////////////////////////////////////////////////////////////////////////////
//// MOUSE INTERFACE
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Shows character panel. Highlights character
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnMouseBeginOverlap(); virtual void OnMouseBeginOverlap_Implementation() override;

	/**
	 * Hides character panel. Stops highlighting character.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnMouseEndOverlap(); virtual void OnMouseEndOverlap_Implementation() override;

	/**
	 * Locks character panel.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnMousePrimary(); virtual void OnMousePrimary_Implementation() override;

	/**
	 * Unlocks panel and custom depth rendering, hides panel and stops rending custom depth.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler Mouse Interface")
	void OnDeselect(); virtual void OnDeselect_Implementation() override;

/////////////////////////////////////////////////////////////////////////////////
//// BLUEPRINT
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Called both on client and server when owner ID changes.
	 * @param NewOwnerID What the new owner ID is.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Autobattler")
	void OnOwnerIDChanged(EEntity NewOwnerID);
	virtual void OnOwnerIDChanged_Implementation(EEntity NewOwnerID) {}

/////////////////////////////////////////////////////////////////////////////////
//// VISUALISATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/**
	 * Updates the health bar values with max and current health.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void UpdateHealthBar();

	/**
	 * Shows/hides health bars for both server and client.
	 * @param IsVisible True to show, false to hide.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void ToggleHealthBarVisibility(bool IsVisible);

	/**
	 * Updates charge bar values with max and current charges.
	 */
	void UpdateChargeBar();

	/**
	 * Updates current budget on UI. 
	 */
	void UpdateCurrentBudget();

	/**
	 * Use autobattler InGameUILogicProvider to see if we want to initially show UI elements or not.
	 * @return Whether to initially show UI elements or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool GetHealthBarInitiallyVisible();

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Network scaffolding.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Updates health bar.
	 */
	UFUNCTION()
	void OnRep_CurrentHealth();

	/**
	 * Updates health bar.
	 */
	UFUNCTION()
	void OnRep_MaxHealth();

	/**
	 * TODO: Implement UI which displays charges.
	 */
	UFUNCTION()
	void OnRep_GainCharges();

	/**
	 * TODO: Implement UI which displays charges.
	 */
	UFUNCTION()
	void OnRep_MaxCharges();

	/**
	 * Updates current budged. 
	 */
	UFUNCTION()
	void OnRep_BudgetChanged();

	/**
	 * Calls BP virtual function to notify owner has been changed.
	 * This can serve to update materials or other visual aspects in blueprints.
	 */
	UFUNCTION()
	void OnRep_OwnerID();

	/**
	 * Ensures both client and server know this character should perform a skill.
	 * @param Animation Animation being performed.
	 * @param ActionSpeed Speed of the animation.
	 * @param ActionType Whether this is an attack or skill.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateExecuteAbilityAnimations(UAnimSequence* Animation, float ActionSpeed, EActionType ActionType);

	/**
	 * Sets the action type for animations only for both server and client.
	 * @param NewActionType Action Type to set. 
	 */
	UFUNCTION(NetMulticast, Reliable)
	void SetAnimationActionType(EActionType NewActionType);

	/* Mark as friend so we can request state changes from this character. */
	friend class UAutobattlerAnimInstance;
	friend class AAutobattlerAIController;

/////////////////////////////////////////////////////////////////////////////////
//// DELEGATE BOUND
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Notifies the AI controller that the game phase has changed.
	 * @param NewGamePhase Game phase which has been changed to. 
	 */
	UFUNCTION()
	void OnGamePhaseChanged(EAutobattlerPhase NewGamePhase);
};

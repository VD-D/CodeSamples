// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerControllerComponent.generated.h"

class AAutobattlerGrid;
class AAutobattlerPawn;
class AAutobattlerPreviewCharacter;
class APlayerController;
class ARedeployPreview;
class UAutobattlerHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIdentityChanged, EEntity, NewIdentity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCooldownEnd, const FName&, AbilityID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityCooldownBegin, const FName&, AbilityID, float, CooldownDuration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMouseToGridLocationChanged, EEntity, WhoChanged, bool, ValidLocation, const FVector&, NewLocation, const FIntPair&, NewGridIndex);

/* Used to enumerate what type of budget we're referring to:
1. Max: total budget.
2. Current: How much is being currently used by deployed units.
3. Pending: How much might be used by a unit ready to deploy. */
UENUM(BlueprintType)
enum class EBudgetUIType : uint8
{
	Max     UMETA(DisplayName = "Max"),
	Current UMETA(DisplayName = "Current"),
	Pending UMETA(DisplayName = "Pending"),
	Count   UMETA(Hidden)
};

/**
 * Component which NEEDS to be put on the player controller for the Autobattler game to work.
 */
UCLASS(ClassGroup=("Autobattler"), meta=(BlueprintSpawnableComponent))
class AUTOBATTLERPLUGIN_API UAutobattlerControllerComponent : public UActorComponent
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DELEGATES
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Called whenever identity changes. Server only. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnIdentityChanged OnIdentityChanged;

	/* Called whenever mouse to grid location changes. Called on both server and client. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnMouseToGridLocationChanged OnMouseToGridLocationChanged;

	/* Called whenever a player ability's cooldown begins. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnAbilityCooldownBegin OnAbilityCooldownBegin;

	/* Called whenever a player ability's cooldown ends. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnAbilityCooldownEnd OnAbilityCooldownEnd;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Which collision channel should be used for any mouse traces. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Autobattler")
	TEnumAsByte<ECollisionChannel> MouseCollisionChannel;

	/* Actor class to use for redeploy preview visualisation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Autobattler")
	TSubclassOf<ARedeployPreview> RedeployPreviewClass;

	/* Class of Autobattler HUD UI. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Autobattler")
	TSubclassOf<UAutobattlerHUD> AutobattlerHUDWidgetClass;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Identity of this controller component. */
	UPROPERTY(Replicated)
	EEntity Identity;

	/* Last grid snapped location. */
	FVector PreviousSnappedLocation;

	/* Widget which allows selection of characters to deploy. */
	UPROPERTY()
	UAutobattlerHUD* AutobattlerHUDWidget;

	/* Character currently being "floated" (previewed). */
	UPROPERTY(Replicated)
	AAutobattlerPreviewCharacter* PreviewCharacter;

	/* Grid which is used for deployment. */
	UPROPERTY()
	AAutobattlerGrid* GameGrid;

	/* An already deployed character currently being dragged (only valid if we have no preview character). */
	UPROPERTY()
	AAutobattlerCharacter* CurrentDraggedCharacter;

	/* Actor visualising redeployment. */
	UPROPERTY(Replicated)
	ARedeployPreview* RedeployPreview;

	FVector LastRedeploymentLocation;
	FIntPair LastRedeploymentIndex;

	/* Used for determining if player abilities can be used. */
	EAutobattlerPhase CurrentPhase;

/////////////////////////////////////////////////////////////////////////////////
//// PLAYER ABILITY
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Array of cosmetic aspects of player abilities for this controller. */
	UPROPERTY(ReplicatedUsing="OnRep_PlayerAbilitiesCosmetic")
	TArray<FAutobattlerPlayerAbilityCosmetic> PlayerAbilitiesCosmetic;

	/* Array of data driving player abilities for this controller. */
	UPROPERTY()
	TArray<FAutobattlerPlayerAbilityData> PlayerAbilitiesData;

	/* Set of abilities which are currently on cooldown. */
	UPROPERTY()
	TSet<FName> AbilitiesOnCooldown;

	/* Whether we are currently trying to target a character. */
	UPROPERTY(ReplicatedUsing="OnRep_IsTargeting")
	bool IsTargeting;

	/* The current ability being targeted (only relevent if IsTargeting = true) */
	UPROPERTY()
	FName TargetedAbilityID;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	UAutobattlerControllerComponent();

	/**
	 * Should only be set on server. The identity (player 1, 2, etc.) of this controller component.
	 * @param NewIdentity Identity to set.
	 */
	void SetIdentity(EEntity NewIdentity);

	/**
	 * Binds relevant delegates to relevant functions.
	 */
	UFUNCTION()
	void BindManagerDelegates();

protected:
	/**
	 * Used for UI construction. 
	 */
	virtual void BeginPlay() override;

/////////////////////////////////////////////////////////////////////////////////
//// USER INPUTS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Context-sensitive primary action. E.g. If we are floating a character, place it.
	 */
	void ExecutePrimaryAction();

	/**
	 * Context-sensitive secondary action. E.g. If we are floating a character, cancel placement.
	 */
	void ExecuteSecondaryAction();

private:
	/**
	 * Context-sensitive primary action. E.g. If we are floating a character, place it.
	 * Called on server.
	 */
	UFUNCTION(Server, Reliable)
	void Server_ExecutePrimaryAction(const FHitResult& MouseHitResult);

	/**
	 * Context-sensitive secondary action. E.g. If we are floating a character, cancel its placement.
	 * Called on server.
	 */
	UFUNCTION(Server, Reliable)
	void Server_ExecuteSecondaryAction(EEntity WhoCalled, const FHitResult& HitUnderMouse);

/////////////////////////////////////////////////////////////////////////////////
//// TICK
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Used to update mouse-to-grid position.
	 * @param DeltaTime Unused.
	 * @param TickType Unused.
	 * @param ThisTickFunction Unused.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

/////////////////////////////////////////////////////////////////////////////////
//// MANAGER DELEGATE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Called when character is added. Updates UI.
	 * @param ID New ID of character.
	 * @param NewListing Listing to create character from.
	 * @param WhoOwns Owner ID.
	 */
	UFUNCTION()
	void OnCharacterAdded(int32 ID, const FCharacterListing& NewListing, EEntity WhoOwns, bool DidGeneratePanelActor);

	/**
	 * Called when character is removed. Updates UI.
	 * @param ID New ID of character.
	 * @param WhoOwns Owner ID.
	 */
	UFUNCTION()
	void OnCharacterRemoved(int32 ID, EEntity WhoOwns, bool WasReturnedToBarracks);

	/**
	 * Called when game phase advances. Updates UI.
	 * @param NewPhase Phase of the game.
	 */
	UFUNCTION()
	void OnGamePhaseAdvanced(EAutobattlerPhase NewPhase);

	/**
	 * Called when float state begins. "Float state" refers to when the owner player is previewing ("floating") a character which is ready to deploy.
	 * Updates UI.
	 * @param WhoOwns For whom the float state was changed.
	 * @param CharacterID ID of character for whom floated state begun.
	 */
	UFUNCTION()
	void OnFloatStateBegun(EEntity WhoOwns, int32 CharacterID);

	/**
	 * Called when float state is cancelled. "Float state" refers to when the owner player is previewing ("floating") a character which is ready to deploy.
	 * Updates UI.
	 * @param WhoOwns For whom the float state was cancelled.
	 * @param FloatedCharacterID ID of floated character.
	 * @param FloatedCharacterListing Character listing to reconstruct cancelled character from.
	 */
	UFUNCTION()
	void OnFloatStateCancelled(EEntity WhoOwns, int32 FloatedCharacterID, const FCharacterListing& FloatedCharacterListing);

	/**
	 * Called when budget updates. Budget, in the context of the autobattler, means a total number of "points," which each units cost. Entities (AI and players) cannot deploy
	 * more than their budget can afford.
	 * Updates UI.
	 * @param WhoOwns For who the budget was updated.
	 * @param NewBudget What the value of the budget now is.
	 */
	UFUNCTION()
	void OnBudgetUpdate(EEntity WhoOwns, int32 NewBudget);

	/**
	 * Called when a character is deployed. Updates UI.
	 * @param WhoOwns Who owns the character.
	 * @param DeployedCharacter Newly deployed character.
	 */
	UFUNCTION()
	void OnCharacterDeployed(EEntity WhoOwns, AAutobattlerCharacter* DeployedCharacter);

/////////////////////////////////////////////////////////////////////////////////
//// OTHER DELEGATE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Called whenever preview character changes placement reason.
	 * @param WhoOwns Who owns the preview character.
	 * @param PlacementReasonChange What the current placement reason is.
	 */
	UFUNCTION()
	void PlacementReasonChange(EEntity WhoOwns, EPlaceReason PlacementReasonChange);

/////////////////////////////////////////////////////////////////////////////////
//// PLAYER ABILITY
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Adds a new player ability to the list of player abilities.
	 * @param AbilityName Row name of the ability, retrieved from Autobattler Config.
	 * SEE: UAutobattlerConfiguration::PlayerAbilitiesDatatable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool AddPlayerAbility(const FName& AbilityName);

	/**
	 * Removes an existing player ability from the list of player abilities.
	 * @param AbilityName Name of the ability from the list of player abilities.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool RemovePlayerAbility(const FName& AbilityName);

	/**
	 * Can be called from either server or client to request executing a player ability.
	 * @param AbilityName of the ability from the list of player abilities.
	 */
	void RequestExecutePlayerAbility(const FName& AbilityName);

	/**
	 * Only relevant if we're currently manually targeting for an ability which requires so.
	 * Attempts to confirm the ability and execute it (fails if the target is invalid).
	 * @param MouseHitResult Hit result under mouse.
	 */
	void ConfirmTargetedAbility(const FHitResult& MouseHitResult);

	/**
	 * Only relevant if we're currently manually targeting for an ability which requires so.
	 * Cancels the pending targeted ability.
	 */
	void CancelTargetedAbility();

private:
	/**
	 * Actually executes a given player ability, if the player has it. Depending on context, will either enter targeting mode 
	 * if manual targeting is required or execute it automatically if auto targeting is used.
	 * @param AbilityName Name of the ability from the list of player abilities.
	 */
	UFUNCTION(Server, Reliable)
	void Server_ExecutePlayerAbility(const FName& AbilityName);

	/**
	 * Attempts to execute pending ability if we are currently targeting and game phase is battle. 
	 */
	UFUNCTION(Server, Reliable)
	void Server_ConfirmTargetedAbility(const FHitResult& MouseHitResult);

	/**
	 * Attempts to cancel pending ability if we are currently targeting.
	 */
	UFUNCTION(Server, Reliable)
	void Server_CancelTargetedAbility();

	/**
	 * Utility function only ever called on the server which spawns the execute ability actor from an implementation
	 * and executes skill list on targets.
	 * @param AbilityName Ability to use.
	 * @param Target Character who is the target of the skill.
	 * NOTE: Area-targeting skills simply target Targets within their range.
	 */
	void SpawnAndExecutePlayerAbility(const FName& AbilityName, AAutobattlerCharacter* Target);

	/**
	 * Sets cooldown on skill and calls delegate.
	 * @param AbilityName Name of ability to set on cooldown.
	 * @param CooldownDuration How long the cooldown should last.
	 */
	void BeginSkillCooldown(const FName& AbilityName, float CooldownDuration);

	/**
	 * Called when a skill cooldown ends. Removes skill from skill cooldown list.
	 * @param AbilityName Skill whose cooldown expired.
	 */
	UFUNCTION()
	void OnSkillCooldownEnd(const FName& AbilityName);

/////////////////////////////////////////////////////////////////////////////////
//// UI
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Shows or hides all elements of the autobattler UI.
	 * @param ShouldBeVisible True shows all UI elements, false hides all of them.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ToggleAutobattlerUI(bool ShouldBeVisible);

	/**
	 * Requests an update for the budget UI elements.
	 * @param BudgetUIType Which UI type we want to update.
	 * @param NewValue What the new value will be.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void RequestBudgetUIUpdate(EBudgetUIType BudgetUIType, int32 NewValue);

	/**
	 * Requests having the visibility of the pending budget change.
	 * @param NewVisibility True to make visible, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void RequestPendingBudgetUIVisibilityChange(bool NewVisibility);

private:
	/**
	 * Helper function for updating the UI regarding player abilities.
	 */
	UFUNCTION()
	void UpdatePlayerAbilityUI();

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Network scaffolding.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * Updates UI on client.
	 */
	UFUNCTION()
	void OnRep_PlayerAbilitiesCosmetic();

	/**
	 * Configures targeting logic on client.
	 */
	UFUNCTION()
	void OnRep_IsTargeting();

/////////////////////////////////////////////////////////////////////////////////
//// CONNECTING AND LOADING
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Attempts to possess the autobattler pawn whose identity matches the identity of this
	 * controller component.
	 */
	bool PossessAutobattlerPawn();

/////////////////////////////////////////////////////////////////////////////////
//// GAME
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Server-side only, attempts to spawn a "floated" (preview character).
	 * @param CharacterListing Listing to spawn from.
	 * @return True if successfully spawned.
	 */
	bool SpawnPreviewCharacter(const FCharacterListing& CharacterListing, int32 ID);

	/**
	 * Server-side only, attempts to clear the preview character.
	 * @return True if clearing was successful.
	 */
	bool ClearPreviewCharacter();

/////////////////////////////////////////////////////////////////////////////////
//// RPC
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Broadcasts on server that this controller's mouse to grid location changed. 
	 * @param WhoChanged The identity of this component.
	 * @param ValidLocation Whether mouse to grid location is valid.
	 * @param NewLocation New mouse to grid location.
	 * @param NewGridIndex New mouse to grid index.
	 */
	UFUNCTION(Server, Reliable, Category = "Autobattler")
	void RPC_MouseToGridLocationChanged(EEntity WhoChanged, bool ValidLocation, const FVector& NewLocation, const FIntPair& NewGridIndex);

	/**
	 * Analagous to AAutobattlerManager::BeginFloatingCharacterForPlayer.
	 * Begins "floating" (previewing deployment) for a character owned by a given player. Calls on server
	 * if this function is called from client.
	 * Cannot be done if there is a battle ongoing.
	 * @param WhoOwns Who this character is owned by.
	 * @param CharacterID ID of the character we wish to float.
	 */
	UFUNCTION(Server, Reliable, Category = "Autobattler")
	void RPC_BeginFloatingCharacterForPlayer(EEntity WhoOwns, int32 CharacterID);

private:
	/**
	 * Called whenever this player uses an ability and it is put on cooldown.
	 * @param AbilityID Unique ID of this ability.
	 * @param Cooldown Duration of cooldown.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void RPC_OnAbilityCooldownBegin(const FName& AbilityID, float Cooldown);

	/**
	 * Called whenever an ability comes off cooldown.
	 * @param AbilityID Unique ID of this ability.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void RPC_OnAbilityCooldownEnd(const FName& AbilityID);

	/**
	 * Clears character panels on both server and client.
	 * @param Force Will remove all character panels, ignoring ID.
	 * @param CharacterID Will only remove character panels with the given ID.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void RPC_ClearCharacterPanels(bool Force, int32 CharacterID);

	/**
	 * Enables/disables the player being able to execute their abilities
	 * (This is actually cosmetic, additional checks are done by the execute functions).
	 * @param NewEnabled True enables, false disables.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void RPC_SetPlayerAbilityEnabledState(bool NewEnabled);

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Attempts to get the player controller owner of this component.
	 * @return The Player Controller which owns this component, or null if this component is not owned by a player controller. 
	 */
	APlayerController* GetPlayerControllerOwner() const { return Cast<APlayerController>(GetOwner()); }

	/**
	 * Attempts to get the autobattler pawn possessed by the player controller owner.
	 * @return The Autobattler Pawn which is possessed by the owner of this component, or null if this component is not owned by a player controller or the controlled pawn is not an AutobattlerPawn.
	 */
	AAutobattlerPawn* GetAutobattlerPawn() const;

	/**
	 * Getter for the preview character. May be null if no preview character exists.
	 * @return The preview character if it exists, null otherwise.
	 */
	AAutobattlerPreviewCharacter* GetPreviewCharacter() const { return PreviewCharacter; }

	/**
	 * Getter for HUD widget on this component. Is valid on server and client.
	 * @return Autobattler HUD widget.
	 */
	UAutobattlerHUD* GetAutobattlerHUDWidget() const { return AutobattlerHUDWidget; }

	/**
	 * Static getter to get controller component by identity.
	 * @param WhoOwns Which identity owns the controller component.
	 * @return Found autobattler controller component, nullptr otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler", meta = (WorldContext = "WorldContextObject"))
	static UAutobattlerControllerComponent* GetControllerComponentByID(const UObject* WorldContextObject, EEntity WhoOwns);

	/**
	 * Static getter to get controller component from context. I.e. on server, this is the server's controller,
	 * on client this is the client's controller.
	 * @return Found autobattler controller component, nullptr otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler", meta = (WorldContext = "WorldContextObject"))
	static UAutobattlerControllerComponent* GetControllerComponentByContext(const UObject* WorldContextObject);

	/**
	 * Static getter for local player controller.
	 * @return Local Player controller, e.g. on a client it will be the client's player controller.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler", meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject);

	/**
	 * Getter for the identity of this controller.
	 * @return Identity of this controller.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EEntity GetIdentity() const { return Identity; }

	/**
	 * Returns the location of the mouse to the grid
	 * @param WorldLocation (OUT) Actual location of the mouse to world.
	 * @param SnappedLocation (OUT) Location of the mouse snapped to the grid.
	 * @param GridIndexLocation (OUT) XY coordinate of location.
	 * @return True if a hit was found and there is a valid grid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool GetMouseToGridLocation(FVector& WorldLocation, FVector& SnappedLocation, FIntPair& GridIndexLocation);

	/**
	 * Getter for the preview character's current transform.
	 * @return PreviewCharacter's transform or default transform if preview character is not valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	FTransform GetPreviewCharacterTransform() const;

	/**
	 * Retrieves data for cosmetic purposes of player ability. Valid on server and client.
	 * @param TargetArray (OUT) Array to be populated.
	 */
	void GetCosmeticPlayerAbilityData(TArray<FAutobattlerPlayerAbilityCosmetic>& TargetArray) { TargetArray = PlayerAbilitiesCosmetic; }

private:
	/**
	 * Utility function which will check validity of game grid and set it
	 * if it is not valid.
	 * @return False if the game grid could not be set, true if it is valid.
	 */
	bool SetGameGrid();
};

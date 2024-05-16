// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerManager.generated.h"

class AAutobattlerCharacter;
class AAutobattlerGrid;
class UAutobattlerConfiguration;
class UBillboardComponent;
class UWinConditionBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseAdvanced, EAutobattlerPhase, NewGamePhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBattleEnded, EWhoWins, WhoWins, const TArray<AAutobattlerCharacter*>&, SurvivingCharacters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDeployed, EEntity, WhoOwns, AAutobattlerCharacter*, DeployedCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFloatStateChanged, EEntity, WhoOwns, int32, FloatedCharacterID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxBudgetChanged, EEntity, WhoOwns, int32, NewMaxBudget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterDeath, EEntity, WhoOwns, int32, DeadCharacterID, AAutobattlerCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFloatStateCancelled, EEntity, WhoOwns, int32, FloatedCharacterID, const FCharacterListing&, FloatedCharacterListing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRemovedCharacterForPlayer, int32, ID, EEntity, WhoOwns, bool, WasReturnedToBarracks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAddedCharacterForPlayer, int32, ID, const FCharacterListing&, NewCharacterListing, EEntity, WhoOwns, bool, DidGeneratePanelActor);

/* Internal structure defining the parameters of an identity (player or enemy). */
USTRUCT()
struct FIdentityConfiguration
{
	GENERATED_BODY()
public:
	/* Where on the grid are we allowed to deploy. Not relevant for player 0, aka the AI. */
	UPROPERTY()
	TArray<FIntPair> AllowedDeploymentIndicies;

	/* The max budget for the identity. Budget, in the context of the autobattler, means a total number of "points," which each units cost. Entities (AI and players) cannot deploy
	more than their budget can afford. */
	UPROPERTY()
	int32 Budget;

	/* Map of unique IDs to the characters they represent. */
	UPROPERTY()
	TMap<int32, FCharacterListing> Characters;
};

/**
 * Actor of which there only should be one per game. Handles managing the autobattler. Most functionality is server-side only.
 */
UCLASS(Blueprintable)
class AUTOBATTLERPLUGIN_API AAutobattlerManager : public AActor
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DELEGATES
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Called on both client and server side when a character is added for a player. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnAddedCharacterForPlayer OnAddedCharacterForPlayer;

	/* Called on both client and server side when a character is removed for a player. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnRemovedCharacterForPlayer OnRemovedCharacterForPlayer;

	/* Called on both client and server side when game phase is changed. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnGamePhaseAdvanced OnGamePhaseAdvanced;

	/* Called on both client and server side when a battle ends (announces winner). */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnBattleEnded OnBattleEnded;

	/* Called on both client and server side when a character dies. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnCharacterDeath OnCharacterDeath;

	/* Called on both client and server side when a character is deployed. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnCharacterDeployed OnCharacterDeployed;

	/* Called on both client and server side when any player begins "floating" (previewing) a character to deploy. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnFloatStateChanged OnFloatBegun;

	/* Called on both client and server side when any player ends "floating" (previewing) a character to deploy. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnFloatStateChanged OnFloatEnd;

	/* Called on both client and server side when any player cancels "floating" (previewing) a character to deploy. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnFloatStateCancelled OnFloatCancelled;

	/* Called both client and server side when max budget changes for an entity. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler")
	FOnMaxBudgetChanged OnMaxBudgetChanged;


/////////////////////////////////////////////////////////////////////////////////
//// COMPONENTS
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Root billboard for finding manager in editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* Billboard;

/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
protected:
	/* Configuration asset used by this manager. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler Config")
	const UAutobattlerConfiguration* AutobattlerConfigurationAsset;

	/* Autobattler grid actor. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler Config")
	AAutobattlerGrid* AutobattlerGrid;

	/* Autobattler grid actor. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler Config")
	TSubclassOf<UWinConditionBase> WinConditionClass;

	/* Whether or not autobattler pawns should be immediately possessed after identities are assigned. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler Config")
	bool PossessAutobattlerPawnsOnAssignIdentity;

	/* When the battle concludes, how long to wait before advacing game phase (this allows animations to play, etc). */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Autobattler Config")
	float BattleEndDelay;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Identity configurations for each identity. */
	UPROPERTY()
	TMap<EEntity, FIdentityConfiguration> IdentityConfigurations;

	/* Used to generate IDs  */
	int32 IDDispenser;

	/* Whether victory should be checked at all (e.g. we may be waiting for battle end delay). */
	bool ShouldCheckVictoryCondition;

	/* Keeps track of current game phase. */
	EAutobattlerPhase GamePhase;

	/* Expected number of players. This will be changed during game initialization */
	int32 NumberOfPlayers = 2;
	/**
	 * @brief Flag informs if setup was already executed locally
	*/
	bool bAlreadySetup = false;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:	
	/**
	 * Default constructor.
	 */
	AAutobattlerManager();

	/**
	 * @brief Called during game initialization
	 * @param InNumberOfPlayers 
	*/
	void Setup(int32 InNumberOfPlayers);

protected:
	/**
	 * Initialises instance. Asserts if there are more than 2 of these in the world.
	 */
	virtual void BeginPlay() override;

private:
	/**
	 * SERVER-ONLY
	 * Assigns identities to the relevant actors in game.
	 * @param WorldContext World which we should search.
	 */
	UFUNCTION()
	void AssignIdentities(UWorld* WorldContext);

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Static getter for the autobattler manager.
	 * @return The autobattler manager.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler", meta = (WorldContext = "WorldContextObject"))
	static AAutobattlerManager* GetManager(const UObject* WorldContextObject);

	/**
	 * Getter for configuration asset.
	 * @return Autobattler configuration asset.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	const UAutobattlerConfiguration* GetAutobattlerConfigurationAsset() const { return AutobattlerConfigurationAsset; };

	/**
	 * Getter for the autobattler grid referenced by this manager.
	 * @return The autobattler grid.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	AAutobattlerGrid* GetAutobattlerGrid() const { return AutobattlerGrid; }

	/**
	 * Getter for current autobattler game phase.
	 * @return Current autobattler game phase.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	EAutobattlerPhase GetGamePhase() const { return GamePhase; }

	/**
	 * Getter for the max budget for a given entity.
	 * @param WhoOwns Entity to get budget for.
	 * @return Max budget for the entity.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	int32 GetMaxBudgetForEntity(EEntity WhoOwns) const;

	/**
	 * Getter for the currently deployed budget for a given entity (i.e. how much of the budget has been used up).
	 * @param WhoOwns Entity to get budget for.
	 * @return Current budget for the entity.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	int32 GetCurrentBudgetForEntity(EEntity WhoOwns) const;

	/**
	 * Server only.
	 * Gets a character listing belonging to an identity by a given ID.
	 * @param WhoOwns Identity who owns the character.
	 * @param ID ID to find.
	 * @param FoundListing (OUT) Found character listing.
	 * @return True if a valid listing was found.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetCharacterListingByID(EEntity WhoOwns, int32 ID, FCharacterListing& FoundListing) const;

	/**
	 * Gets all deployed characters belonging to WhoOwns.
	 * @param DeployedCharacters (OUT) Characters deployed who are owned by WhoOwns.
	 * @param WhoOwns Identity who owns the characters.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	void GetDeployedCharactersByEntity(TArray<AAutobattlerCharacter*>& DeployedCharacters, EEntity WhoOwns) const;

	/**
	 * Gets array of IDs of characters which are currently deployed.
	 * @param DeployedCharacterIDs (OUT) IDs of all characters deployed, owned by WhoOwns.
	 * @param WhoOwns Identity who owns the characters.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	void GetIDsOfDeployedCharacters(TArray<int32>& DeployedCharacterIDs, EEntity WhoOwns) const;

	/**
	 * Getter for WhoOwns by a character ID.
	 * @param ID Character ID
	 * @param WhoOwns (OUT) Will be set to who owns if an owner is found.
	 * @return True if there is any owner for the given ID (false means character ID was never registered).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetWhoOwnsByID(int32 ID, EEntity& WhoOwns) const;

	/**
	 * Getter for if a character with a given ID is deployed or not (false implies it is in the barracks).
	 * @param ID ID of the character.
	 * @param DeployedCharacter The character with the given ID if it is deployed, nullptr otherwise.
	 * @return If a character with a given ID is deployed or not (false implies it is in the barracks).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetIsCharacterDeployed(int32 ID, AAutobattlerCharacter*& DeployedCharacter) const;

	/**
	 * Server-only.
	 * Checks to see if a deployment index is occupied by another (already deployed) character .
	 * @param IndexToTest Index we want to check is occupied.
	 * @return True if occupied, false if not occupied (always false on client).
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetIsGridIndexOccupied(const FIntPair& IndexToTest) const;

	/**
	 * Checks to see if any autobattler characters exist which block CollisionChannel.
	 * @param BoxCenter Bounds center.
	 * @param BoxExtent Half size of box.
	 * @param CollisionChannel Collision channel to test against.
	 * @param ActorsToIgnore Any actors which should be ignored in the trace.
	 * @return True if any autobattler character exists within the region, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetIsBoundsOccupied(const FVector& BoxCenter, const FRotator& BoxRotation, const FVector& BoxExtent, TEnumAsByte<ECollisionChannel> CollisionChannel, const TArray<AActor*>& ActorsToIgnore) const;

	/**
	 * Server-only.
	 * Checks to see if WhoOwns can deploy on the IndexToTest.
	 * @param WhoOwns "Owner" or player who checks where they can deploy.
	 * @param IndexToTest Index we want to check can be deployed on.
	 * @return True if we can deploy on that index
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler")
	bool GetCanPlayerDeployOnIndex(EEntity WhoOwns, const FIntPair& IndexToTest) const;

/////////////////////////////////////////////////////////////////////////////////
//// GAME
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * SERVER-ONLY. 
	 * Advances game phase. Setup goes to battle, battle to end and end to setup.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void AdvanceGamePhase();

	/**
	 * SERVER-ONLY.
	 * Checks to see if either side has won.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void CheckWinCondition();

	/**
	 * Sets the max budget for an entity.
	 * @param WhoOwns Entity to set budget for.
	 * @param NewBudget Max budget to set. Clamped to be value equal to or greater than 1.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void SetBudgetForEntity(EEntity WhoOwns, int32 NewBudget);

	/**
	 * Adds/subtracs budget for an entity.
	 * @param WhoOwns Entity to set budget for.
	 * @param DeltaBudget How much budget to add or remove. Cannot drop below 0 budget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ModifyBudgetForEntity(EEntity WhoOwns, int32 DeltaBudget);

	/**
	 * SERVER-ONLY
	 * Sets a deployment index a player can deploy to (this is irrelevant for player 0, i.e. the AI).
	 * @param WhoOwns Which identity to add deployment index for.
	 * @param NewIndex New deployment index to add.
	 * @return Whether adding was successful or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool AddDeploymentIndexForPlayer(EEntity WhoOwns, const FIntPair& NewIndex);

	/**
	 * SERVER-ONLY
	 * Removes a deployment index a player can deploy to (this is irrelevant for player 0, i.e. the AI).
	 * @param WhoOwns Which identity to remove deployment index for.
	 * @param IndexToRemove Deployment index to remove.
	 * @return Whether removal was successful or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool RemoveDeploymentIndexForPlayer(EEntity WhoOwns, const FIntPair& IndexToRemove);

	/**
	 * SERVER-ONLY
	 * Adds a character which can be deployed for a given identity.
	 * @param NewListing Character listing to generate new character from.
	 * @param WhoOwns Who this character is owned by.
	 * @param ID (OUT) Unique identity of this character (used, for example, when removing or deploying characters).
	 * @param GeneratePanelActor Whether we want to create a panel actor so a character panel widget will be displayed for this character. (@see CharacterPanelActor.h)
	 * @return Whether the character was successfully added or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool AddCharacterForPlayer(const FCharacterListing& NewListing, EEntity WhoOwns, int32& ID, bool GeneratePanelActor = true);

	/**
	 * SERVER-ONLY
	 * Adds a character which can be deployed for a given identity. Analagous to AddCharacterForPlayer, but
	 * adds only by row name (no modifiers)
	 * @param NewRowName Row name to generate new character from.
	 * @param WhoOwns Who this character is owned by.
	 * @param ID (OUT) Unique identity of this character (used, for example, when removing or deploying characters).
	 * @param GeneratePanelActor Whether we want to create a panel actor so a character panel widget will be displayed for this character. (@see CharacterPanelActor.h)
	 * @return Whether the character was successfully added or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool AddCharacterForPlayerNoModifiers(const FName& NewRowName, EEntity WhoOwns, int32& ID, bool GeneratePanelActor = true);

	/**
	 * SERVER-ONLY
	 * Removes a character by ID for a given player.
	 * @param WhoOwns Who this character is owned by.
	 * @param ID ID of character to remove.
	 * @param ReturnToBarracks If false, the character is removed permanently. If true, it is returned to barracks and can be re-deployed.
	 * NOTE: If you attempt to remove a character in the barracks while ReturnToBarracks is true, nothing happens.
	 * @return True if removal was successful, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool RemoveCharacterForPlayer(EEntity WhoOwns, int32 ID, bool ReturnToBarracks = false);

	/**
	 * SERVER-ONLY
	 * Similar to RemoveCharacterForPlayer, except uses CharacterToRemove to figure out owner and ID.
	 * @param CharacterToRemove Used to get ID and owner.
	 * @param ReturnToBarracks If false, the character is removed permanently. If true, it is returned to barracks and can be re-deployed.
	 * NOTE: If you attempt to remove a character in the barracks while ReturnToBarracks is true, nothing happens.
	 * @return True if removal was successful, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool RemoveCharacterForPlayerByContext(AAutobattlerCharacter* CharacterToRemove, bool ReturnToBarracks = false);

	/**
	 * SERVER-ONLY
	 * Begins "floating" (previewing deployment) for a character owned by a given player. Calls on server
	 * if this function is called from client.
	 * Cannot be done if there is a battle ongoing.
	 * @param WhoOwns Who this character is owned by.
	 * @param CharacterID ID of the character we wish to float.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void BeginFloatingCharacterForPlayer(EEntity WhoOwns, int32 CharacterID);

	/**
	 * SERVER-ONLY
	 * Cancels "floating" (previewing deployment) for a character owned by a given player. 
	 * Cannot be done if there is a battle ongoing.
	 * @param WhoOwns Who this character is owned by.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Autobattler")
	bool CancelFloatingCharacterForPlayer(EEntity WhoOwns);

	/**
	 * SERVER-ONLY.
	 * Deploys an existing autobattler character for a player.
	 * Cannot be done if there is a battle ongoing.
	 * @param WhoOwns Who this character is owned by.
	 * @param CharacterID ID of the character we wish to deploy.
	 * @param GridIndex This will be converted to a location to deploy at.
	 * @param Rotation Character deploy rotation.
	 * @param Scale Character deploy scale.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool DeployCharacterForPlayerByGridIndex(EEntity WhoOwns, int32 CharacterID, const FIntPair& GridIndex, const FRotator& Rotation, const FVector& Scale);

	/**
	 * SERVER-ONLY.
	 * Deploys an existing autobattler character for a player.
	 * Cannot be done if there is a battle ongoing.
	 * @param WhoOwns Who this character is owned by.
	 * @param CharacterID ID of the character we wish to deploy.
	 * @param DeployTransform Transform we want to deploy with.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	bool DeployCharacterForPlayerByTransform(EEntity WhoOwns, int32 CharacterID, const FTransform& DeployTransform);

	/**
	 * Generates an AI formation and deploys the AI.
	 * @param FormationRowName Formation to read from configuration.
	 * @param MaxNumEnemies Max number enemies which will be placed (may be less if formation is less than this number).
	 * @param WhiteList Characters with tags on whitelist will be considered for deployment.
	 * @param Blacklist Characters with tags on blacklist will never be considered for deployment.
	 * @return Whether a formation was successfully generated.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler|AI")
	bool GenerateAIFormation(const FName& FormationRowName, const TArray<FName>& WhiteList, const TArray<FName>& Blacklist, bool ClearPrevious = false);

	/**
	 * Generates a battle from a debug configuration (used for testing purposes).
	 * @param DebugBattleRowName Row name of config from debug config datatable.
	 * @return True if the battle was successfully generated.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler|AI")
	bool GenerateBattleFromDebugConfig(const FName& DebugBattleRowName);

	/**
	 * Generates an AI formation from a formation datatable entry.
	 * @param FormationDatatable Table to generate formation from. Must have rowtype AutobattlerAIFormation.
	 * @param RowName Name of row in datatable.
	 * @return True if the formation was generated successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler|AI")
	bool GenerateAIFormationFromDatatable(const UDataTable* FormationDatatable, const FName& RowName);

	/**
	 * Clears the battlefield of soldiers (removes all Autobattler Characters).
	 * @param ReturnToBarracks If true, soldiers removed will be returned to their respective barracks. Otherwise, all deployed soldiers will be removed permanently.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ClearBattlefield(bool ReturnToBarracks = false);

	/**
	 * This does the following:
	 * Clears battlefield.
	 * Removes all soldier from all barracks.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ClearAutobattlerState();

	/**
	 * This completely clears a state for a given entity.
	 * 1. All deployment indicies are removed.
	 * 2. Budget is set to 0.
	 * 3. All soldiers are removed from all barracks and the battlefield.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void ClearStateForEntity(EEntity WhoOwns);

	/**
	 * Requests changing the visibility of the a grid for a given entity. Indicies shown
	 * depending on their configuration.
	 * @param WhoOwns Entity we want change grid visualisation for.
	 * @param bGridVisible Whether the grid is meant to be visible or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void RequestChangeGridVisualisationForEntity(EEntity WhoOwns, bool bGridVisible);


	/**
	 * SERVER ONLY
	 * @brief This should be called when game is initializing/loading
	 * @param InNumberOfPlayers
	*/
	void SetNumberOfPlayers(int32 InNumberOfPlayers);

private:
	/**
	 * Internal function used for checking win condition when action type changes
	 * and when checking if a character dies.
	 * @param NewAction Check to see if is dead.
	 * @param UpdatedCharacter Character for whom action was updated for.
	 */
	UFUNCTION()
	void OnAnyCharacterStateChange(EActionType NewAction, AAutobattlerCharacter* UpdatedCharacter);

	/**
	 * This simply executes a battle end sequence after a delay.
	 */
	UFUNCTION()
	void DelayedBattleEnd();

	/**
	 * Hanldes cleanup when any character is destroyed.
	 */
	UFUNCTION()
	void OnCharacterDestroyed(AActor* DestoryedActor);

	/**
	 * Utility to generate tags for a formation
	 * @param ChosenTags Tags which are selected.
	 * @param RandomiserParams Params for choosing tags.
	 */
	void GenerateFormationTags(TArray<FName>& ChosenTags, const TArray<FTagRandomiserParameters>& RandomiserParams);

/////////////////////////////////////////////////////////////////////////////////
//// NETWORKING
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Called whenever a character is successfully added for a player.
	 * Used for cosmetic purposes only, e.g. updating UI.
	 * @param NewCharacterDefinition Definition of character which was added.
	 * @param WhoOwns Who this character is owned by.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnAddCharacterForPlayer(int32 ID, const FCharacterListing& NewCharacterListing, EEntity WhoOwns, bool DidGeneratePanelActor);

	/**
	 * Called whenever a character is successfully removed for a player.
	 * Used for cosmetic purposes only, e.g. updating UI.
	 * @param ID ID of removed character.
	 * @param WhoOwns Who this character is owned by.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnRemoveCharacterForPlayer(int32 ID, EEntity WhoOwns, bool WasReturnedToBarracks);

	/**
	 * Called whenever the game phase advances.
	 * @param NewPhase New game phase.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnGamePhaseAdvance(EAutobattlerPhase NewPhase);

	/**
	 * Called whenever a battle ends (and a winner is announced).
	 * @param WhoWins Which side won the battle.
	 * @param Survivors Which characters survived the battle.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnBattleEnd(EWhoWins WhoWins, const TArray<AAutobattlerCharacter*>& Survivors);

	/**
	 * Called whenever a character is deployed.
	 * @param WhoOwns Owner of the character.
	 * @param DeployedCharacter Newly deployed character. 
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnCharacterDeploy(EEntity WhoOwns, AAutobattlerCharacter* DeployedCharacter);

	/**
	 * Called whenever the float state changes.
	 * @param WhoOwns Who this character is owned by.
	 * @param Begun Whether we began or ended floating.
	 * @param CharacterID Character ID character who begun/end floating.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnFloatStateChange(EEntity WhoOwns, bool Begun, int32 CharacterID);

	/**
	 * Called whenever float state is cancelled.
	 * @param WhoOwns Who this character is owned by.
	 * @param CharacterID Character ID of character whose float state was cancelled.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnFloatStateCancelled(EEntity WhoOwns, const FCharacterListing& FloatedCharacterListing, int32 CharacterID);

	/**
	 * Called whenever max budget changes.
	 * @param WhoOwns For who the budget has changed.
	 * @param NewBudget Value of new budget.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnMaxBudgetChange(EEntity WhoOwns, int32 NewBudget);

	/**
	 * Called whenever we want the grid to change its visual state.
	 * @param Grid The grid itself.
	 * @param WhoOwns For which player to change visual state for.
	 * @param Show Whether to show (true) or hide completely (false).
	 * @param IndiciesToShow Which grid indicies to show if Show = true.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_RequestIndexVisibilityChange(AAutobattlerGrid* Grid, EEntity WhoOwns, bool Show, const TArray<FIntPair>& IndiciesToShow);

	/**
	 * Called whenever a character dies.
	 * @param WhoOwns For which player the character died for.
	 * @param DeadCharacterID ID of dead character.
	 * @param DeadCharacter Actual character who died.
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_OnCharacterDeath(EEntity WhoOwns, int32 DeadCharacterID, AAutobattlerCharacter* DeadCharacter);

	/**
	 * Clears invalid character panels on both client and server. 
	 */
	UFUNCTION(NetMulticast, Reliable, Category = "Autobattler")
	void Multicast_ClearInvalidCharacterPanels();

/////////////////////////////////////////////////////////////////////////////////
//// DEBUG
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Prints the configuration of the given entity to the autobattler log.
	 * @param WhoOwns Which entity to print configuration.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler")
	void PrintEntityConfigurationToLog(EEntity WhoOwns);
};

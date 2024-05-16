// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EQSQueryResultSourceInterface.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerAIController.generated.h"

class AAutobattlerCharacter;
class UAutobattlerSkill;
class UBehaviorTree;
class UBehaviorTreeComponent;
class UEQSRenderingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillExecuted, EAbilityType, AbilityExecuted);

/* Internal use only enum. */
enum class ELastQueryResultType : uint8
{
	PathAround,
	PointsAround,
	None
};

/**
 * AI controller for all autobattler pawns.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API AAutobattlerAIController : public AAIController, public IEQSQueryResultSourceInterface
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DELEGATES
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Delegate which executes when either an ability or attack is executed. */
	UPROPERTY(BlueprintAssignable, Category = "Autobattler AI")
	FOnSkillExecuted OnSkillExecuted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEQSRenderingComponent* RenderComp;

/////////////////////////////////////////////////////////////////////////////////
//// INTERNAL
/////////////////////////////////////////////////////////////////////////////////
private:
	/* Holds current game phase. */
	EAutobattlerPhase CurrentPhase;

	/* Character listing of parent character. */
	FName CharacterListingName;

	/* Whether we want to execute an ability on the next cycle. */
	bool IsAbilityQueued;
	bool UsesAbilityOnly;

	/* Whether the AI should attempt an update. */
	bool ShouldUpdate;
	EAbilityType LastRelevantActionType;

	/* Values which must be checked to see if they are still current per AI Update. */
	bool IsMovingToTarget;
	FAbilityTargetingProperties TargetingProperties;
	FAbilityTargetingProperties PreviousTargetingProperties;
	AAutobattlerCharacter* CurrentTarget;

	/* Actual move to location. */
	FVector DesiredMoveToLocation;

	/* Marked as friend so it can update states. */
	friend class AAutobattlerCharacter;

	/* EQS Debug. */
	bool IsDebugEnabled;
	FEnvQueryResult* LastQueryResult;
	TSharedPtr<FEnvQueryInstance> QueryInstance;
	ELastQueryResultType LastQueryResultType = ELastQueryResultType::None;

	/* How often query items can be drawn and how many. */
	const float RedrawQueryItemsFrequenty = 1.0f;
	bool CanDrawDebugItems = true;
	const int32 MaxItemsToDraw = 25;

/////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 * @param ObjectInitializer Unused.
	 */
	AAutobattlerAIController(const FObjectInitializer& ObjectInitializer);

protected:
	/**
	 * Initialise behavior tree.
	 */
	virtual void BeginPlay() override;

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * This should be called whenever the character is in the middle of doing something and so
	 * should stop updating any properties. 
	 * @param NewShouldUpdate True if AI updates should be fired, false otherwise.
	 */
	void SetShouldUpdate(bool NewShouldUpdate) { ShouldUpdate = NewShouldUpdate; }

	/**
	 * This should be called whenever the character's listing name changes.
	 * @param NewListingName Listing name to set.
	 */ 
	void SetCharacterListingName(const FName& NewListingName) { CharacterListingName = NewListingName; }

	/**
	 * Setter for previous targeting properties.
	 * @param NewPreviousTargetingProps New properties to set.
	 */
	void SetPreviousTargetingProperties(const FAbilityTargetingProperties& NewPreviousTargetingProps) { PreviousTargetingProperties = NewPreviousTargetingProps; }

	/**
	 * Sets the last target character found by this AI controller.
	 * @param NewLastTarget New target character found by this AI controller.
	 */
	void SetLastTarget(AAutobattlerCharacter* NewLastTarget) { CurrentTarget = NewLastTarget; }

	/**
	 * Enables/disables debugging for this AI controller.
	 * @param NewEnabled Whether debugging should be enabled.
	 */
	void SetDebugEnabled(bool NewEnabled);

	/**
	 * Gets the last known targeting properties.
	 * @param TargetingProps (OUT) Populated with last known targeting properties.
	 */
	void GetPreviousTargetingProperties(FAbilityTargetingProperties& TargetingProps) const { TargetingProps = PreviousTargetingProperties; }

	/**
	 * Gets the current targeting properties.
	 * @param TargetingProps (OUT) Populated with current targeting properties.
	 */
	void GetCurrentTargetingProperties(FAbilityTargetingProperties& TargetingProps) const { TargetingProps = TargetingProperties; }

	/**
	 * Gets actual desired move to location.
	 * @return Desired move to location.
	*/
	FVector GetDesiredMoveToLocation() const { return DesiredMoveToLocation; }

	/**
	 * Getter for whether an ability is queued.
	 * @return Whether an ability is queued.
	 */
	bool GetIsAbilityQueued() const { return IsAbilityQueued; }

	/**
	 * Getter for whether this AI controller only ever executes an ability.
	 * @return Whether this AI controller only ever executes an ability.
	 */
	bool GetUsesAbilityOnly() const { return UsesAbilityOnly; }

	/**
	 * Gets whether or not the AI should ever update.
	 * @return Whether or not the AI should ever update.
	 */
	bool GetAIShouldEverUpdate() const;

	/**
	 * Gets the targeting properties for a given skill implementation.
	 * @param Skill Skill to get targeting properties from.
	 * @return Targeting properties for the skill.
	 */
	FAbilityTargetingProperties GetAbilityTargetingProperties(const UAutobattlerSkill* Skill);

	/**
	 * Gets either the attack or ability implementation.
	 * @param SkillType Whether this should return the Attack or Ability implementation.
	 * @return Attack or ability implementation.
	 */
	const UAutobattlerSkill* GetRelevantSkill(EAbilityType SkillType);

	/**
	 * Gets the last target character found by this AI controller.
	 * @return Last target character found by this AI controller.
	 */
	AAutobattlerCharacter* GetLastTarget() const { return CurrentTarget; }

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

	/**
	 * Will continuously perform an "AI update," checking targeting properties and ranges,
	 * but will never interrupt an already occuring action. 
	 */
	UFUNCTION()
	void AIUpdate();

	/**
	 * Called whenever the ability goes off cooldown. 
	 */
	UFUNCTION()
	void OnAbilityCooldownEnd();

	/**
	 * Query result for path around query. Tries to move actor to nearest location where it can
	 * proceed in a straight line to the target.
	 * @param Result Query result.
	 */
	void PathAroundQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	/**
	 * Query result for surrounding points query. Checks if there exists a straight path between this
	 * and target location.
	 * @param Result Query result.
	 */
	void SurroundingPointsQueryFinished(TSharedPtr<FEnvQueryResult> Result);

/////////////////////////////////////////////////////////////////////////////////
//// AI
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Rotates character toward target.
	 * @param DeltaTime Used to interp.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * This should be called when relevant animations have finished and our state should "reset."
	 */
	void ResetState();

	/**
	 * Resets movement state by halting movement and setting IsMovingToTarget to false.
	 */
	void ResetMovementState();

	/**
	 * Tries to get most relevant skill and executes it.
	 * Stops AI updating.
	 */
	void TryExecuteRelevantSkill();

	/**
	 * Attempts to execute the relevant skill.
	 */
	void StartExecuteSkill(const UAutobattlerSkill* SkillToExecute);

	/**
	 * This should be called from the Anim Notify of when the skill should trigger.
	 */
	void ExecuteSkill(EAbilityType AbilityTypeTriggered);

	/**
	 * This should be called from the Anim Notify when we want to transition back to idle.
	 */
	void EndSkill();

/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
private:
	/**
	 * Utility to get the Autobattler character controlled by this AI controller.
	 */
	AAutobattlerCharacter* GetControlledCharacter() const;

	/**
	 * Utility to get the spawn location of the next ability.
	 * @param Transform (OUT) Spawn transform of next ability.
	 * @param SkillTriggerType Whether the skill is instant, projectile or effect.
	 * @param SocketName Only relevant if trigger type is proectile. Optional socket name to spawn at.
	 */
	void GetNextSkillSpawnLocation(FTransform& Transform, ESkillTriggerEnd SkillTriggerType, const FName& SocketName);

	/**
	 * Utility to draw env query as debug shapes.
	 * @param QueryResult Query result to draw items for.
	 */
	void DrawEnvQueryItems(FEnvQueryResult* QueryResult); //const TArray<FEnvQueryItem>& Items);

	/**
	 * Allows drawing debug items again. (Called on timer). 
	 */
	UFUNCTION()
	void ResetCanDrawDebugItems();

/////////////////////////////////////////////////////////////////////////////////
//// EQS QUERY RESULTS
/////////////////////////////////////////////////////////////////////////////////
public:
	virtual const FEnvQueryResult* GetQueryResult() const override;
	virtual const FEnvQueryInstance* GetQueryInstance() const override;

	virtual bool GetShouldDebugDrawLabels() const override { return true; }
	virtual bool GetShouldDrawFailedItems() const override{ return true; }
	virtual float GetHighlightRangePct() const override { return 1.0f; };
};

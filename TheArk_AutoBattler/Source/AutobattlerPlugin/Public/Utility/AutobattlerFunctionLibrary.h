// Copyright Juggler Games 2022
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/AutobattlerStructs.h"
#include "AutobattlerFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAutobattler, Log, All);

class AAutobattlerCharacter;
class AAutobattlerGrid;
class APawn;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UENUM(BlueprintType)
enum class EVerbosityLevel : uint8
{
	Log     UMETA(DisplayName = "Log"),
	Warning UMETA(DisplayName = "Warning"),
	Error   UMETA(DisplayName = "Error"),
	Count   UMETA(Hidden)
};

/**
 * Global utility functions for the autobattler plugin.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UAutobattlerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DEBUG
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Utility to make an entity configuration, intended for debug purposes.
	 * @param EntityConfiguration (OUT) The debug configuration.
	 * @param MaxCharactersToDeploy How many characters the entity can deploy.
	 * @param DeploymentRangeMin X/Y min grid indecies which entity can deploy characters on (indicies are between RangeMin and RangeMax)
	 * @param DeploymentRangeMax X/Y max grid indecies which entity can deploy characters on (indicies are between RangeMin and RangeMax)
	 * @param CharacterRowNames Row names of characters available to the entity.
	 * NOTE: This debug configuration does not support modifiers.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void MakeDebugEntityConfigurationNoModifiers(
		FEntityConfiguration& EntityConfiguration,
		int32 MaxCharactersToDeploy,
		const FIntPair& DeploymentRangeMin,
		const FIntPair& DeploymentRangeMax,
		const TArray<FName>& CharacterRowNames
	);

/////////////////////////////////////////////////////////////////////////////////
//// UI
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Attempts to remove all character panels in viewport.
	 * @param Force Removes all, without checking ID.
	 * @param IDToRemove Checks if the panel matches the ID, then removes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library", meta = (WorldContext = "WorldContextObject"))
	static void ClearCharacterPanelsInViewport(const UObject* WorldContextObject, bool Force, int32 IDToRemove);

	/**
	 * Removes all character panels in viewport if its ID does not belong to any existing ID. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library", meta = (WorldContext = "WorldContextObject"))
	static void ClearInvalidCharacterPanels(const UObject* WorldContextObject);

	/**
	 * Changes visibility for all in-game UI (such as health bars). Executes locally, e.g. meaning
	 * if executed on client it will hide in-game UI for the client but not the server.
	 * @param ShouldBeVisible True to show in-game UI, false to hide.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library", meta = (WorldContext = "WorldContextObject"))
	static void ChangeInGameUIVisibility(const UObject* WorldContextObject, bool ShouldBeVisible);

/////////////////////////////////////////////////////////////////////////////////
//// ACCESSORS
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Utility which tries to find the character definition for the character's datatable
	 * definined in the AutobattlerConfiguration asset.
	 * @param RowName Row name of the character.
	 * @return The definition from that row name, or nullptr if such a row could not be found. 
	 */
	static FAutobattlerCharacterDefinition* GetCharacterDefinitionFromConfigurationDatatable(const UObject* WorldContextObject, const FName& RowName);

	/**
	 * Tries to get the blackboard component from the PawnOwner.
	 * @param PawnOwner Pawn to get blackboard from
	 * @return Blackboard component if pawn has valid AI controller, nullptr otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static UBlackboardComponent* GetPawnBlackboardComponent(const APawn* PawnOwner);

	/**
	 * Tries to get the pawn owner of a behaviour tree.
	 * @return The pawn owner of the behaviour tree, or null if it does not have one. 
	 */
	static APawn* GetBehaviourTreePawnOwner(UBehaviorTreeComponent& OwnerComp);

	/**
	 * Checks if one entity is the enemy of another. Players (i.e. entities not 0) are not considered enemies.
	 * @param EntityOne First entity to test.
	 * @param EntityTwo Second entity to test.
	 * @return True if the two entities are enemies.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Library")
	static bool IsEnemy(EEntity OwnerOne, EEntity OwnerTwo);

	/**
	 * Gets all characters in the world filtered by FilterType.
	 * @param FilteredCharacters All characters filtered by FilterType.
	 * @param ContextCharacter "Self." Usually the character who owns a skill or attack.
	 * @param FilterType Which characters should be filtered.
	 * @param FilterDead Whether characters which are dead should be filtered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void GetCharactersFiltered(TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter* ContextCharacter, EAbilityFilterType FilterType, bool FilterDead = true);

	/**
	 * Gets all characters in the box filtered by FilterType.
	 * @param FilteredCharacters All characters in the box filtered by FilterType.
	 * @param ContextCharacter "Self." Usually the character who owns a skill or attack.
	 * @param CharacterCollisionObjectType Object type to search for (usually Pawn).
	 * @param BoxOrigin World location of box origin.
	 * @param BoxExtent Extent of the box. 
	 * @param FilterType Which characters should be filtered.
	 * @param FilterDead Whether characters which are dead should be filtered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library", meta = (WorldContext = "WorldContextObject"))
	static void GetCharactersFilteredInBox(TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter* ContextCharacter, TEnumAsByte<ECollisionChannel> CharacterCollisionObjectType, const FVector& BoxOrigin, const FVector& BoxExtent, EAbilityFilterType FilterType, bool FilterDead = true);

	/**
	 * Gets all characters in the sphere filtered by FilterType.
	 * @param FilteredCharacters All characters in the box filtered by FilterType.
	 * @param ContextCharacter "Self." Usually the character who owns a skill or attack.
	 * @param CharacterCollisionObjectType Object type to search for (usually Pawn).
	 * @param SphereOrigin World location of sphere origin.
	 * @param SphereRadius Radius of the sphere.
	 * @param FilterType Which characters should be filtered.
	 * @param FilterDead Whether characters which are dead should be filtered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library", meta = (WorldContext = "WorldContextObject"))
	static void GetCharactersFilteredInSphere(TArray<AAutobattlerCharacter*>& FilteredCharacters, AAutobattlerCharacter* ContextCharacter, TEnumAsByte<ECollisionChannel> CharacterCollisionObjectType, const FVector& SphereOrigin, float SphereRadius, EAbilityFilterType FilterType, bool FilterDead = true);

private:
	/**
	 * Internal use only. The actual "algorithm" for filtering characters, used by various filter functions.
	 * @param FilteredCharacters All filtered characters.
	 * @param CharacterToChooseFrom Array of characters which should actually be queried.
	 * @param ContextCharacter "Self." Usually the character who owns a skill or attack.
	 * @param FilterType Which character's should be filtered.
	 * @param FilterDead Whether characters which are dead should be filtered.
	 */
	static void CharacterFilterAlgorithm(TArray<AAutobattlerCharacter*>& FilteredCharacters, const TArray<AAutobattlerCharacter*>& CharacterToChooseFrom, AAutobattlerCharacter* ContextCharacter, EAbilityFilterType FilterType, bool FilterDead = true);

/////////////////////////////////////////////////////////////////////////////////
//// MATH
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Gets the manhattan (taxcab) distance between A and B.
	 * @param A Point A.
	 * @param B Point B.
	 * @return Manhattan distance between the two points.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Library")
	static int32 ManhattanDistance(const FIntPair& A, const FIntPair& B);

	/**
	 * Gets the manhattan (taxcab) distance between A and B.
	 * @param A Point A.
	 * @param B Point B.
	 * @return Manhattan distance between the two points.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Library")
	static int32 VManhattanDistance(const FVector& A, const FVector& B);

	/**
	 * Get the cartesian (3D) distance between A and B.
	 * @param A Point A.
	 * @param B Point B.
	 * @return Cartesian distance between the two points.
	 */
	UFUNCTION(BlueprintPure, Category = "Autobattler Library")
	static float CartesianDistance(const FVector& A, const FVector& B);

/////////////////////////////////////////////////////////////////////////////////
//// UTILITY
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Makes a character base stats struct from a definition.
	 * @param Definition Character definition.
	 * @return Character base stats struct.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library", meta = (WorldContext = "WorldContextObject"))
	static FCharacterBaseStats MakeBaseStatsFromDefinition(const UObject* WorldContextObject, const FAutobattlerCharacterDefinition& Definition);

	/**
	 * Converts a text into an array separated by delimiter.
	 * @param SourceText Text to parse into array.
	 * @param Delimiter Element which divides text.
	 * @param Elements (OUT) Parsed elements.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void ParseTextIntoStringArray(const FText& SourceText, const FString& Delimiter, TArray<FName>& Elements);

	/**
	 * Prints a string to either screen or log.
	 * @param Message Message to print.
	 * @param VerbosityLevel Verbosity of the message.
	 * @param ToScreen Whether to print to screen.
	 * @param ToLog Whether to print to log.
	 * @param Key Only relevant if ToScreen is true. Message key.
	 * @param Time Only relevant if ToScreen is true. How long the message should be displayed for.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void PrintMessage(const FString& Message, EVerbosityLevel VerbosityLevel = EVerbosityLevel::Log, bool ToScreen = true, bool ToLog = false, int32 Key = -1, float Time = 5.0f);

	/**
	 * Wrapper around PrintMessage which prints an error to the log.
	 * @param Message Message to print.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void PrintErrorToLog(const FString& Message);

	/**
	 * Wrapper around PrintMessage which prints a warning to the log.
	 * @param Message Message to print.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void PrintWarningToLog(const FString& Message);

	/**
	 * Wrapper around PrintMessage which prints a message to the log.
	 * @param Message Message to print.
	 */
	UFUNCTION(BlueprintCallable, Category = "Autobattler Library")
	static void PrintMessageToLog(const FString& Message);

	/**
	 * Gets the first component of the given type on an actor.
	 * @param Actor The actor to search components of.
	 * @return The first found component of the given type, or nullptr if one does not exist.
	 */
	template<class T>
	static T* GetFirstComponent(const AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;

		TArray<T*> Components;
		Actor->GetComponents<T>(Components);
		return Components.IsValidIndex(0) ? Components[0] : nullptr;
	}

	/**
	 * Checks if the actor has at least one component of the given type.
	 * @param Actor The actor to search components of.
	 * @return True if at least one component of the given type exists.
	 */
	template<class T>
	static bool HasComponent(const AActor* Actor)
	{
		if (!IsValid(Actor)) return false;

		TArray<T*> Components;
		Actor->GetComponents<T>(Components);
		return Components.IsValidIndex(0);
	}
};

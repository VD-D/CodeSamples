// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SacrusMordumSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSubLevelUnlocked
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Sub-Level Unlocked")
	FName SublevelName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Sub-Level Unlocked")
	bool IsUnlocked;

	/*
	FSubLevelUnlocked(FName NewSublevelName, bool NewIsUnlocked)
	{
		SublevelName = NewSublevelName;
		IsUnlocked = NewIsUnlocked;
	}

	FORCEINLINE bool operator==(const FSubLevelUnlocked& Right)
	{
		return Right.SublevelName.IsEqual(SublevelName) && Right.IsUnlocked == IsUnlocked;
	}
	*/
};

/* Save game class for the Sacrus Mordum game. */
UCLASS()
class SACRUSMORDUM_API USacrusMordumSaveGame : public USaveGame
{
	GENERATED_BODY()
/* --- SAVE GAME VARIABLES --- */
public:
	/* Data which is used when creating a new save game. */
	// UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Sacrus Mordum Save Game")
	TMap<FName, TArray<FSubLevelUnlocked>> InitialUnlockedLevelData;

private:
	/* Container for storing unlocked levels. The top-level key is level names while the
	bottom-level key is the sublevel name, and true if it has been unlocked. */
	TMap<FName, TArray<FSubLevelUnlocked>> SavedLevelData;

/* --- FUNCTIONS --- */
public:
	/** Constructor with some default values.
	  */
	USacrusMordumSaveGame();

	/** Tries to load if sub levels are unlocked for a given level.
	  * @param LevelName - Name of the level.
	  * @param UnlockedSublevels - (mutable) Populated with unlocked sublevels. Only modified if parent level exists.
	  * @return True if the level was found, false otherwise.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Save Game")
	bool GetUnlockedSublevels(FName LevelName, TArray<FSubLevelUnlocked>& FoundUnlockedSublevels);

	/** Gets all unlocked levels.
	  * @param UnlockedLevels - (mutable) Populated with unlocked levels. If a level name is absent, then the level has not been unlocked.
	  */
	UFUNCTION(BlueprintCallable, Category = "Sacrus Mordum Save Game")
	void GetAllUnlockedLevels(TArray<FName>& UnlockedLevels);
};

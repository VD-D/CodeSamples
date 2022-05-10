// Copyright Robert Zygmunt Uszynski 2020-2021

#include "SaveGameRelated/SacrusMordumSaveGame.h"

USacrusMordumSaveGame::USacrusMordumSaveGame()
{
	/*
	TArray<FSubLevelUnlocked> InitialUnlockedSublevels;
	InitialUnlockedSublevels.Add(FSubLevelUnlocked(FName("LithyrlStAparment_2030"), true));
	InitialUnlockedSublevels.Add(FSubLevelUnlocked(FName("LithyrlStAparment_0615"), false));
	InitialUnlockedLevelData.Add(FName("LithyrlStApartment"), InitialUnlockedSublevels);
	*/
}

bool USacrusMordumSaveGame::GetUnlockedSublevels(FName LevelName, TArray<FSubLevelUnlocked>& FoundUnlockedSublevels)
{
	TArray<FSubLevelUnlocked>* NewUnlockedSublevels = SavedLevelData.Find(LevelName);
	if (NewUnlockedSublevels == nullptr) return false;

	FoundUnlockedSublevels = *NewUnlockedSublevels;
	return true;
}

void USacrusMordumSaveGame::GetAllUnlockedLevels(TArray<FName>& UnlockedLevels)
{
	SavedLevelData.GenerateKeyArray(UnlockedLevels);
}

// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "../SaveGameRelated/SacrusMordumSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "CameraRollWidget.generated.h"

/* Widget that exists in on the computer screen and while viewing footage,
giving the illusion that they are the same. */
UCLASS()
class SACRUSMORDUM_API UCameraRollWidget : public UUserWidget
{
	GENERATED_BODY()
/* --- FUNCTIONS --- */
protected:
	/** Called whenever the camera roll widget begins loading data from a save file.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera Roll Widget")
	void OnBeginSaveDataLoad();
	virtual void OnBeginSaveDataLoad_Implementation();
	
	/** Called whenever the camera roll widget finishes loading data from a save file.
	  * @param LoadLevelName - The name of the level being loaded.
	  * @param LoadSublevelNames - The sublevel names of the loaded level.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera Roll Widget")
	void OnEndSaveDataLoad(FName LoadLevelName, const TArray<FSubLevelUnlocked>& LoadSublevelNames);
	virtual void OnEndSaveDataLoad_Implementation(FName LoadLevelName, const TArray<FSubLevelUnlocked>& LoadSublevelNames);
};

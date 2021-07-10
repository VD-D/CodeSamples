// Copyright Robert Zygmunt Uszynski 2019-2020

#pragma once

#include "CoreMinimal.h"
#include "Enumerations/Gait.h"
#include "Enumerations/Stance.h"
#include "UObject/Interface.h"
#include "PlayerStateInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerStateInterface : public UInterface
{
	GENERATED_BODY()
};

/* Any entities which need to react to the player changing a state (such as switching from crouching
to uncrouching or vice versa) should implement this interface. */
class LOCOMOTION_API IPlayerStateInterface
{
	GENERATED_BODY()

public:
	/** Called when the player crouches/uncrouched.
	  * @param HasEnteredCrouching - True if the player has just crouched, otherwise false.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerStateChange")
	void OnPlayerCrouch(bool& HasEnteredCrouching);

	/** Called when the player's forced movement models are updated.
	  * @param NewAllowedStances - Array of EStance which the player is presently allowed to use.
	  * @param NewAllowedGaits - Array of EGait which the player is presently allowed to use.
	  */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerStateChange")
	void OnPlayerMovementModelsUpdate(const TArray<EStance>& NewAllowedStances, const TArray<EGait>& NewAllowedGaits);
};

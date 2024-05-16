// Copyright Juggler Games 2022 - 2023
// Contributors: Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Skills/SkillImplementation.h"
#include "PrintMessage.generated.h"

/**
 * Execute skill implementation which prints a message to the screen.
 * Development only.
 */
UCLASS()
class AUTOBATTLERPLUGIN_API UPrintMessage : public USkillImplementation
{
	GENERATED_BODY()
public:
	/**
	 * Prints a message to string. Parameters are irrelevant.
	 * @param SkillOwner Used to get name of owner.
	 * @param SkillTargetingMode Unused.
	 * @param Target Unused.
	 * @param TargetLocation Unused.
	 */
	virtual void ExecuteSkill_Implementation(AAutobattlerCharacter* SkillOwner, ESkillTargetingMode SkillTargetingMode, AAutobattlerCharacter* Target, const FVector& TargetLocation) override;
	
/////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION
/////////////////////////////////////////////////////////////////////////////////
public:
	/* Message will be printed in format: _NAME_OF_ACTOR_ : Message. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Print Message")
	FString Message;
	
	/* How long the message should appear for. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Print Message")
	float Duration = 5.0f;
};

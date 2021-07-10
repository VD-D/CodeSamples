// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "PlayerInfiltrationController.generated.h"

/* Player controller for the infiltration module. */
UCLASS()
class INFILTRATION_API APlayerInfiltrationController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* The "team" which this player controller is on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Infiltration Controller")
	uint8 TeamID;

private:
	/* Storage of GenericTeamId */
	FGenericTeamId GenericTeamId;

/* --- FUNCTIONS --- */
public:
	/**
	 * Constructor.
	 */
	APlayerInfiltrationController();

	/**
	 * Sets a new team id.
	 * @param NewID - The new team id to set. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Player Infiltration Controller")
	void SetNewTeamId(uint8 NewID);

protected:
	/**
	 * Changes the player's TeamID if it is not 0. 
	 */
	virtual void BeginPlay() override;

private:
	/**
	 * Override of GetGenericTeamId() from IGenericTeamAgentInterface. Returns this controller's team ID.
	 * @return This controller's team ID;
	 */
	virtual FGenericTeamId GetGenericTeamId() const override; 
};

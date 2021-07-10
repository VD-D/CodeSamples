// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerInfiltrationController.h"

APlayerInfiltrationController::APlayerInfiltrationController()
{
    /* Reasonable defaults */
    TeamID = 0;
    GenericTeamId = FGenericTeamId(0);
}

void APlayerInfiltrationController::SetNewTeamId(uint8 NewID)
{
    TeamID = NewID;
    GenericTeamId = FGenericTeamId(NewID);
}

void APlayerInfiltrationController::BeginPlay()
{
    if (TeamID != 0) SetNewTeamId(TeamID);
}

FGenericTeamId APlayerInfiltrationController::GetGenericTeamId() const
{
    return GenericTeamId;
}

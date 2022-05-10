// Copyright Robert Zygmunt Uszynski 2020-2021


#include "Investigation/InvestigationManager.h"

// Sets default values
AInvestigationManager::AInvestigationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInvestigationManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInvestigationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


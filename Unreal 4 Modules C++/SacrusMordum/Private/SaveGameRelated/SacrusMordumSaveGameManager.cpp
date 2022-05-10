// Copyright Robert Zygmunt Uszynski 2020-2021

#include "SaveGameRelated/SacrusMordumSaveGameManager.h"

#include "Components/BillboardComponent.h"

ASacrusMordumSaveGameManager::ASacrusMordumSaveGameManager()
{
	/* Component creation. */
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;

	/* No tick required.*/
	PrimaryActorTick.bCanEverTick = false;
}

void ASacrusMordumSaveGameManager::BeginPlay()
{
	Super::BeginPlay();
}

// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InvestigationManager.generated.h"

/* Actor which manages the investigation screen. */
UCLASS()
class SACRUSMORDUM_API AInvestigationManager : public AActor
{
	GENERATED_BODY()

/* --- FUNCTIONS --- */
public:	
	/* --- CONSTRUCTION --- */

	/** Constructor.
	  */
	AInvestigationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

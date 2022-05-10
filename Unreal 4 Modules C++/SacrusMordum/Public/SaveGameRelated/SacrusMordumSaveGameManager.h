// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SharedStructures/DatatableStructures.h"
#include "SacrusMordumSaveGameManager.generated.h"

/* Class which manages unlocks and progression in Sacrus Mordum. */
UCLASS()
class SACRUSMORDUM_API ASacrusMordumSaveGameManager : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */

	/* Billboard component to make this actor easily identifiable in editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBillboardComponent* Billboard;

/* --- FUNCTIONS --- */
public:	
	/** Constructor.
	  */
	ASacrusMordumSaveGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

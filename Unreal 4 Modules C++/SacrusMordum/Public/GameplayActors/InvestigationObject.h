// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "SharedStructures/InvestigationInfoProperties.h"
#include "GameFramework/Actor.h"
#include "InvestigationObject.generated.h"

UCLASS()
class SACRUSMORDUM_API AInvestigationObject : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* --- COMPONENTS --- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Investigation Character Components")
	class UBoxComponent* InvestigationObjectBounds;

	/* --- CONFIGURABLE --- */

	/* The datatable from which this investigation object's properties should be read from. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object|Configurable")
	class UDataTable* InvestigationObjectDatatable;

protected:
	/* Row name of the investigation object, to retrieve it from the datatable. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Investigation Object|Configurable")
	FName InvestigationObjectRowName;

/* --- FUNCTIONS --- */
public:	
	/* --- CONSTRUCTION --- */

	/** Constructor.
	  */
	AInvestigationObject();

	/* --- GETTERS --- */

	/** Retrieves InvestigationObjectProperties for this object based on its name and the associated datatable.
	  */
	UFUNCTION(BlueprintCallable, Category = "Investigation Object|Getters", meta = (DisplayName = "Get Object Info"))
	FInvestigationObjectProperties GetInvestigationObjectInformation();

	/* --- WIDGETS --- */

	/** Function which creates an "info" widget, giving information about the investigation object in question.
	  */
	UFUNCTION(BlueprintCallable, Category = "Investigation Object|Widgets", meta = (DisplayName = "Create Info Widget"))
	void CreateInfoWidget();
};

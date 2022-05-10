// Copyright Robert Zygmunt Uszynski 2020-2021

#include "GameplayActors/InvestigationObject.h"
#include "Components/BoxComponent.h"
#include "../../SacrusMordum.h"

AInvestigationObject::AInvestigationObject()
{
	/* By default, the investigation object bounds has no collision, since it really should only be set to overlap
	a designer-chosen channel for investigation objects. */
	InvestigationObjectBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Investigation Object Bounds"));
	InvestigationObjectBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InvestigationObjectBounds->ShapeColor = FColor((uint8) 0, (uint8) 0, (uint8) 255);
	RootComponent = InvestigationObjectBounds;

	/* Tick is not required. */
	PrimaryActorTick.bCanEverTick = false;
}

FInvestigationObjectProperties AInvestigationObject::GetInvestigationObjectInformation()
{
	/* When retrieving InvestigationObjectProperties, if the datatable is null, log a warning and return
	the default constructed investigation object properties. */
	if (InvestigationObjectDatatable == nullptr)
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Datatable for Investigation Object %s is null, returning default Investigation Properties!"), *InvestigationObjectRowName.ToString())
		return FInvestigationObjectProperties();
	}

	/* Try to find the row in the datatable from the provided name. If this fails, log a warning and return 
	the default constructed investigation object properties. */
	FInvestigationObjectProperties* NewInvestigationObjectProperties = InvestigationObjectDatatable->FindRow<FInvestigationObjectProperties>(InvestigationObjectRowName, FString("Investigation Object"));

	if (NewInvestigationObjectProperties == nullptr)
	{
		UE_LOG(LogSacrusMordum, Warning, TEXT("Cannot find row %s for investigation object, returning default Investigation Properties!"), *InvestigationObjectRowName.ToString())
		return FInvestigationObjectProperties();
	}

	/* If everything succeeds, return the retrieved the dereferenced investigation object properties. */
	return *NewInvestigationObjectProperties;
}

void AInvestigationObject::CreateInfoWidget()
{
}

FInvestigationObjectProperties::FInvestigationObjectProperties()
{
	ObjectName.BaseString = FString("Sample format {0}");
	ObjectName.Format.Add(FString("Text"));
}
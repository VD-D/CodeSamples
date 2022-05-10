// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "CustomisableCharacter/CustomisableCharacter.h"
#include "InvestigationPersonality.generated.h"

UENUM(BlueprintType)
enum class EInvestigationStatus : uint8
{
	Discovered UMETA(DisplayName = "Discovered"),
	Known UMETA(DisplayName = "Known"),
	Unknown UMETA(DisplayName = "Unknown")
};

/* A personality which can be "investigated". Has the functionality to zoom the camera in on it,
and can be discovered/known/unknown. */
UCLASS()
class SACRUSMORDUM_API AInvestigationPersonality : public ACustomisableCharacter
{
	GENERATED_BODY()
/* --- COMPONENTS --- */
public:
	/* The material which replaces all materials on apparel and the body to "hide" the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Character")
	class UMaterialInterface* HideMaterial;

	/* This personality's current investigation status. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Character")
	EInvestigationStatus InvestigationStatus;

/* --- FUNCTIONS --- */
protected:
	/** Override of construction script is necessary to prevent automatic character rebuilding.
	  * @param Transform - Unused.
	  */
	virtual void OnConstruction(const FTransform& Transform) override;
	
};

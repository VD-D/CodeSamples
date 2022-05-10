// Copyright Robert Zygmunt Uszynski 2020-2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InvestigationInfoProperties.generated.h"

USTRUCT(BlueprintType)
struct FStringFormatter
{
	GENERATED_BODY()
public:
	/* Base string. To use Format text, it should be used like "Name : {0} Age : {1}." */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	FString BaseString;

	/* Arguments to pass to format. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	TArray<FString> Format;

	/* Any additional information which the specific bit of information may have. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	TArray<FName> Tags;
};

USTRUCT(BlueprintType)
struct FInvestigationObjectProperties : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Name of the object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	FStringFormatter ObjectName;

	/* Information about the object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	TArray<FStringFormatter> ObjectInformation;

	/* The "icon" of the object rendered on a Canvas Target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	class UTextureRenderTarget2D* ObjectDisplayRender;

	/* Any additional tags which these investigation object propeties may have. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Object Properties")
	TArray<FName> Tags;

	/* Default constructor. */
	FInvestigationObjectProperties();
};

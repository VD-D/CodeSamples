// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterCustomiserStructures.generated.h"

UENUM(BlueprintType)
enum class EBodyMaterialType : uint8
{
	Skin UMETA(DisplayName = "Skin"),
	Eyes UMETA(DisplayName = "Eyes"),
	Eyebrows UMETA(DisplayName = "Eyebrows"),
	Eyelashes UMETA(DisplayName = "Eyelashes"),
	Eyeshadow UMETA(DisplayName = "Eyeshadow")
};

UENUM(BlueprintType)
enum class EHairType : uint8
{
	Hair UMETA(DisplayName = "Hair"),
	Beard UMETA(DisplayName = "Beard"),
	FacialHair UMETA(DisplayName = "Facial Hair")
};

UENUM(BlueprintType)
enum class EApparelType : uint8
{
	Head UMETA(DisplayName = "Head"),
	Body UMETA(DisplayName = "Body"),
	Legs UMETA(DisplayName = "Legs"),
	Feet UMETA(DisplayName = "Feet"),
	Arms UMETA(DisplayName = "Arms"),
	Misc UMETA(DisplayName = "Misc")
};

USTRUCT(BlueprintType)
struct FBodyMaterialScalarProperties
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	TMap<FName, float> Parameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	EBodyMaterialType BodyMaterialType;
};

USTRUCT(BlueprintType)
struct FBodyMaterialTextureProperties
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	TMap<FName, class UTexture*> Parameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	EBodyMaterialType BodyMaterialType;
};

USTRUCT(BlueprintType)
struct FBodyMaterialVectorProperties
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	TMap<FName, FVector> Parameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	EBodyMaterialType BodyMaterialType;
};

USTRUCT(BlueprintType)
struct FBodyMaterialProperties
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Material Properties")
	class UMaterialInterface* Material;

	FBodyMaterialProperties()
	{
		MaterialIndex = 0;
		Material = nullptr;
	}

	FBodyMaterialProperties(int32 Index, class UMaterialInterface* Mat)
	{
		MaterialIndex = Index;
		Material = Mat;
	}
};

struct FBodyMaterialPropertiesInternal
{
public:
	int32 MaterialIndex;
	class UMaterialInstanceDynamic* Material;
};

USTRUCT(BlueprintType)
struct FMeshProperties
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Apparel Properties")
	class USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Apparel Properties")
	TArray<class UMaterialInterface*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Apparel Properties")
	TMap<FName, float> ScalarParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Apparel Properties")
	TMap<FName, FLinearColor> VectorParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Apparel Properties")
	TMap<FName, class UTexture*> TextureParameters;
};

/* Structure which contains all information necessary to creating a character. */
USTRUCT(BlueprintType)
struct FCustomisableCharacterParameters : public FTableRowBase
{
	GENERATED_BODY()
public:
	/* Map of all morph values to be applied to the mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customisable Character Parameters")
	TMap<FName, float> MorphValues;

	/* All scalar parameters to be applied to the body (skin, eyes, etc.). Key = parameter name, value = body type and value itself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customisable Character Parameters")
	TArray<FBodyMaterialScalarProperties> BodyMaterialScalarValues;

	/* All vector parameters to be applied to the body (skin, eyes, etc.). Key = parameter name, value = body type and value itself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customisable Character Parameters")
	TArray<FBodyMaterialVectorProperties> BodyMaterialVectorValues;

	/* All texture parameters to be applied to the body (skin, eyes, etc.). Key = parameter name, value = body type and value itself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customisable Character Parameters")
	TArray<FBodyMaterialTextureProperties> BodyMaterialTextureValues;

	/* Hair to be applied to the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customisable Character Parameters")
	TMap<EHairType, FMeshProperties> CharacterHair;

	/* Apparel to be applied to the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customisable Character Parameters")
	TMap<EApparelType, FMeshProperties> CharacterApparel;

	/* Constructor. */
	FCustomisableCharacterParameters()
	{
		MorphValues.Add(FName("face_01"), 0.0f);
		MorphValues.Add(FName("face_02"), 0.0f);
		MorphValues.Add(FName("face_03"), 0.0f);
		MorphValues.Add(FName("face_04"), 0.0f);
		MorphValues.Add(FName("face_05"), 0.0f);
		MorphValues.Add(FName("face_06"), 0.0f);
		MorphValues.Add(FName("face_07"), 0.0f);
		MorphValues.Add(FName("face_08"), 0.0f);
		MorphValues.Add(FName("face_09"), 0.0f);

		MorphValues.Add(FName("ears_01"), 0.0f);
		MorphValues.Add(FName("ears_02"), 0.0f);
		MorphValues.Add(FName("ears_03"), 0.0f);
		MorphValues.Add(FName("ears_04"), 0.0f);
		MorphValues.Add(FName("ears_05"), 0.0f);
		MorphValues.Add(FName("ears_06"), 0.0f);
		MorphValues.Add(FName("ears_07"), 0.0f);
		MorphValues.Add(FName("ears_08"), 0.0f);
		MorphValues.Add(FName("ears_09"), 0.0f);

		MorphValues.Add(FName("eyes_01"), 0.0f);
		MorphValues.Add(FName("eyes_02"), 0.0f);
		MorphValues.Add(FName("eyes_03"), 0.0f);
		MorphValues.Add(FName("eyes_04"), 0.0f);
		MorphValues.Add(FName("eyes_05"), 0.0f);
		MorphValues.Add(FName("eyes_06"), 0.0f);
		MorphValues.Add(FName("eyes_07"), 0.0f);
		MorphValues.Add(FName("eyes_08"), 0.0f);
		MorphValues.Add(FName("eyes_09"), 0.0f);

		MorphValues.Add(FName("brows_01"), 0.0f);
		MorphValues.Add(FName("brows_02"), 0.0f);
		MorphValues.Add(FName("brows_03"), 0.0f);
		MorphValues.Add(FName("brows_04"), 0.0f);
		MorphValues.Add(FName("brows_05"), 0.0f);
		MorphValues.Add(FName("brows_06"), 0.0f);
		MorphValues.Add(FName("brows_07"), 0.0f);
		MorphValues.Add(FName("brows_08"), 0.0f);
		MorphValues.Add(FName("brows_09"), 0.0f);

		MorphValues.Add(FName("mouth_01"), 0.0f);
		MorphValues.Add(FName("mouth_02"), 0.0f);
		MorphValues.Add(FName("mouth_03"), 0.0f);
		MorphValues.Add(FName("mouth_04"), 0.0f);
		MorphValues.Add(FName("mouth_05"), 0.0f);
		MorphValues.Add(FName("mouth_06"), 0.0f);
		MorphValues.Add(FName("mouth_07"), 0.0f);
		MorphValues.Add(FName("mouth_08"), 0.0f);
		MorphValues.Add(FName("mouth_09"), 0.0f);

		MorphValues.Add(FName("nose_size"), 0.0f);
		MorphValues.Add(FName("nose_curve"), 0.0f);
		MorphValues.Add(FName("nose_tip"), 0.0f);
		MorphValues.Add(FName("nose_tip_angle"), 0.0f);

		MorphValues.Add(FName("eyes_height"), 0.0f);
		MorphValues.Add(FName("eyes_depth"), 0.0f);
		MorphValues.Add(FName("eyes_size"), 0.0f);
		MorphValues.Add(FName("eyes_slant"), 0.0f);
		MorphValues.Add(FName("eyes_narrow"), 0.0f);

		MorphValues.Add(FName("mouth_size"), 0.0f);
		MorphValues.Add(FName("mouth_height"), 0.0f);
		MorphValues.Add(FName("mouth_depth"), 0.0f);
		MorphValues.Add(FName("nasolabial_fold"), 0.0f);
		MorphValues.Add(FName("jaw_height"), 0.0f);
		MorphValues.Add(FName("jaw_width"), 0.0f);
		MorphValues.Add(FName("underbite"), 0.0f);
		MorphValues.Add(FName("chin_size"), 0.0f);
		MorphValues.Add(FName("chin_width"), 0.0f);
		MorphValues.Add(FName("chin_height"), 0.0f);

		MorphValues.Add(FName("brow_height"), 0.0f);
		MorphValues.Add(FName("brow_depth"), 0.0f);
		MorphValues.Add(FName("body_muscular"), 0.0f);
		MorphValues.Add(FName("body_emaciated"), 0.0f);
		MorphValues.Add(FName("body_fat"), 0.0f);
		MorphValues.Add(FName("body_breastsize"), 0.0f);
		MorphValues.Add(FName("body_curvy"), 0.0f);
		MorphValues.Add(FName("body_musclemass"), 0.0f);
		MorphValues.Add(FName("body_age"), 0.0f);
	}
};
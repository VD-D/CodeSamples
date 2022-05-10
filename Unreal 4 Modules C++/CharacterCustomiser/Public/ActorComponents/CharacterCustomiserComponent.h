// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structures/CharacterCustomiserStructures.h"
#include "CharacterCustomiserComponent.generated.h"


UCLASS(ClassGroup=(CharacterCustomiser), meta=(BlueprintSpawnableComponent))
class CHARACTERCUSTOMISER_API UCharacterCustomiserComponent : public UActorComponent
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Used to refresh the character in the editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Refresh")
	bool bRefreshInEditor;

	/* Defines what materials go where on the body of the mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Config")
	TMap<EBodyMaterialType, FBodyMaterialProperties> BodyMaterialProperties;

	/* The datatable defining properties of preset characters. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Config")
	class UDataTable* PresetCharacterDatatable;

	/* The character's name, as it appears in the PresetCharacter datatable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Config")
	FName PresetCharacterRowName;

	/* Name of the skeletal mesh will be set as the master mesh (e.g. on a Character this is "Mesh") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName MasterMeshCompName;

	/* Name of the skeletal mesh which upper body apparel will be applied to (i.e. Shirts) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName UpperBodySkeletalMeshCompName;

	/* Name of the skeletal mesh which lower body apparel will be applied to (i.e. trousers) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName LowerBodySkeletalMeshCompName;

	/* Name of the skeletal mesh which footware apparel will be applied to (i.e. shoes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName FeetSkeletalMeshCompName;

	/* Name of the skeletal mesh which misc apparel will be applied to (i.e. glasses) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName MiscMeshCompName;

	/* Name of the skeletal mesh which hair will be applied to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName HairMeshCompName;

	/* Name of the skeletal mesh which beards will be applied to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName BeardMeshCompName;

	/* Name of the skeletal mesh which mustaches will be applied to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Customiser|Skeletal Mesh Component Names")
	FName MoustacheMeshCompName;

private:
	/* Lookup table for finding skeletal mesh components which have already been found. */
	TMap<FName, class USkeletalMeshComponent*> SkeletalMeshCompLUT;

	/* Internal use map for body materials. */
	TMap<EBodyMaterialType, FBodyMaterialPropertiesInternal> BodyMaterials;

/* --- FUNCTIONS --- */
public:	
	/**
	 * Constructor. 
	 */
	UCharacterCustomiserComponent();

	/** 
	 * Builds a character from a row name and datatable (note that this is automatically done from the construction script).
	 * @param CharacterRowName The character's name as it appears in the datatable.
	 * @param CustomisableCharacterDatatable The datatable itself.
	 * @param ForceMaterialInstanceRebuild Whether or not to force the material instance being rebuild. Editor use only.
	 */
	UFUNCTION(BlueprintCallable, Category = "Customisable Character", meta = (DisplayName = "Build Character from Datatable"))
	void BuildCharacter(FName CharacterRowName, class UDataTable* CustomisableCharacterDatatable, bool ForceMaterialInstanceRebuild = false);

	/** 
	 * Builds a character from the row name and datatable provided as the default or "preset".
	 * @param ForceMaterialInstanceRebuild Whether or not to force the material instance being rebuild. Editor use only.
	 */
	UFUNCTION(BlueprintCallable, Category = "Customisable Character", meta = (DisplayName = "Build Character from Preset"))
	void BuildCharacterFromPreset(bool ForceMaterialInstanceRebuild = false);

	/** 
	 * Adds apparel to a character at the given slot with the given properties. Note that this function is called by BuildCharacter.
	 * @param ApparelSlot The slot where the apparel should be added to (body, legs, etc.)
	 * @param ApparelProperties The properties of the apparel, such as the mesh, its materials and so forth.
	 */
	UFUNCTION(BlueprintCallable, Category = "Customisable Character", meta = (DisplayName = "Apply Apparel to Character"))
	void ApplyApparel(EApparelType ApparelSlot, FMeshProperties ApparelProperties);

	/** 
	 * Adds hair to a character at the given slot with the given properties. Note that this function is called by BuildCharacter.
	 * @param HairSlot The slot where the hair should be added to (hair or beard etc.)
	 * @param HairProperties The properties of the hair, such as the mesh, its materials and so forth.
	 */
	UFUNCTION(BlueprintCallable, Category = "Customisable Character", meta = (DisplayName = "Apply Hair to Character"))
	void ApplyHair(EHairType HairSlot, FMeshProperties HairProperties);

protected:
	/**
	 * Begin play override. 
	 */
	virtual void BeginPlay() override;

private:
	/**
	 * Finds a skeletal mesh component on the owner based on a tag.
	 * @param ComponentTag Tag of the component to find.
	 * @return The skeletal mesh component of the given name, or nullptr if not found. 
	 */
	class USkeletalMeshComponent* FindSkeletalMeshCompByTag(const FName& ComponentTag);

	/**
	 * Applies parameters to a body material.
	 * @param BodyMaterialTypeToApplyTo The body material to apply parameters to.
	 * @param Parameters The parameters to apply. 
	 */
	void ApplyBodyMaterialParameters(EBodyMaterialType BodyMaterialTypeToApplyTo, TMap<FName, float> Parameters);
	void ApplyBodyMaterialParameters(EBodyMaterialType BodyMaterialTypeToApplyTo, TMap<FName, FVector> Parameters);
	void ApplyBodyMaterialParameters(EBodyMaterialType BodyMaterialTypeToApplyTo, TMap<FName, class UTexture*> Parameters);

	/** 
	 * Creates body materials.
	 * @param Force Whether to force material creation if they have already been created.
	 */
	void CreateBodyMaterials(bool Force = false);

	/** 
	 * Applies apparel to a given skeletal mesh component.
	 * @param SkeletalMeshComponent The component to apply apparel to.
	 * @param MeshProperties The apparel to apply.
	 */
	void ApplyMeshPropertiesInternal(class USkeletalMeshComponent* SkeletalMeshComponent, FMeshProperties MeshProperties);
		
};

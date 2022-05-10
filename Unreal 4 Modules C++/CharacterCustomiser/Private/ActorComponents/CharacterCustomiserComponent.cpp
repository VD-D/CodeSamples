// Copyright Robert Uszynski 2021

#include "ActorComponents/CharacterCustomiserComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

UCharacterCustomiserComponent::UCharacterCustomiserComponent()
{
	/* Tick not required. */
	PrimaryComponentTick.bCanEverTick = false;

	/* Reasonable defaults. */
	MasterMeshCompName = FName("Mesh");
	UpperBodySkeletalMeshCompName = FName("UpperBody");
	LowerBodySkeletalMeshCompName = FName("LowerBody");
	FeetSkeletalMeshCompName = FName("Footwear");
	MiscMeshCompName = FName("Misc");
	HairMeshCompName = FName("Hair");
	BeardMeshCompName = FName("Beard");
	MoustacheMeshCompName = FName("Moustache");

	BodyMaterialProperties.Add(EBodyMaterialType::Skin, FBodyMaterialProperties(0, nullptr));
	BodyMaterialProperties.Add(EBodyMaterialType::Eyes, FBodyMaterialProperties(1, nullptr));
	BodyMaterialProperties.Add(EBodyMaterialType::Eyeshadow, FBodyMaterialProperties(2, nullptr));
	BodyMaterialProperties.Add(EBodyMaterialType::Eyebrows, FBodyMaterialProperties(3, nullptr));
	BodyMaterialProperties.Add(EBodyMaterialType::Eyelashes, FBodyMaterialProperties(4, nullptr));
}

void UCharacterCustomiserComponent::BuildCharacter(FName CharacterRowName, UDataTable * CustomisableCharacterDatatable, bool ForceMaterialInstanceRebuild)
{
	if (CustomisableCharacterDatatable == nullptr) return;

	USkeletalMeshComponent* MasterMeshComp = FindSkeletalMeshCompByTag(MasterMeshCompName);
	if (MasterMeshComp == nullptr) return;

	FCustomisableCharacterParameters* NewCharacterParams = CustomisableCharacterDatatable->FindRow<FCustomisableCharacterParameters>(CharacterRowName, FString("Character Customiser"));
	if (NewCharacterParams == nullptr) return;

	CreateBodyMaterials(ForceMaterialInstanceRebuild);

	for (auto BodyMaterial : BodyMaterials)
	{
		for (auto ScalarParams : NewCharacterParams->BodyMaterialScalarValues)
		{
			if (ScalarParams.BodyMaterialType == BodyMaterial.Key) ApplyBodyMaterialParameters(ScalarParams.BodyMaterialType, ScalarParams.Parameters);
		}

		for (auto VectorParams : NewCharacterParams->BodyMaterialVectorValues)
		{
			if (VectorParams.BodyMaterialType == BodyMaterial.Key) ApplyBodyMaterialParameters(VectorParams.BodyMaterialType, VectorParams.Parameters);
		}

		for (auto TextureParams : NewCharacterParams->BodyMaterialTextureValues)
		{
			if (TextureParams.BodyMaterialType == BodyMaterial.Key) ApplyBodyMaterialParameters(TextureParams.BodyMaterialType, TextureParams.Parameters);
		}
	}

	for (auto MorphTarget : NewCharacterParams->MorphValues)
	{
		MasterMeshComp->SetMorphTarget(MorphTarget.Key, MorphTarget.Value);
	}

	for (auto CharacterHairPiece : NewCharacterParams->CharacterHair)
	{
		ApplyHair(CharacterHairPiece.Key, CharacterHairPiece.Value);
	}

	for (auto CharacterApparelPiece : NewCharacterParams->CharacterApparel)
	{
		ApplyApparel(CharacterApparelPiece.Key, CharacterApparelPiece.Value);
	}
}

void UCharacterCustomiserComponent::BuildCharacterFromPreset(bool ForceMaterialInstanceRebuild)
{
	BuildCharacter(PresetCharacterRowName, PresetCharacterDatatable, ForceMaterialInstanceRebuild);
}

void UCharacterCustomiserComponent::ApplyApparel(EApparelType ApparelSlot, FMeshProperties MeshProperties)
{
	switch (ApparelSlot)
	{
	case EApparelType::Body:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(UpperBodySkeletalMeshCompName), MeshProperties);
		break;
	case EApparelType::Feet:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(FeetSkeletalMeshCompName), MeshProperties);
		break;
	case EApparelType::Legs:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(LowerBodySkeletalMeshCompName), MeshProperties);
		break;
	case EApparelType::Misc:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(MiscMeshCompName), MeshProperties);
		break;
	}
}

void UCharacterCustomiserComponent::ApplyHair(EHairType HairSlot, FMeshProperties HairProperties)
{
	switch (HairSlot)
	{
	case EHairType::Hair:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(HairMeshCompName), HairProperties);
		break;
	case EHairType::Beard:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(BeardMeshCompName), HairProperties);
		break;
	case EHairType::FacialHair:
		ApplyMeshPropertiesInternal(FindSkeletalMeshCompByTag(MoustacheMeshCompName), HairProperties);
		break;
	}
}

void UCharacterCustomiserComponent::BeginPlay()
{
	Super::BeginPlay();
}

USkeletalMeshComponent* UCharacterCustomiserComponent::FindSkeletalMeshCompByTag(const FName& ComponentTag)
{
	if (SkeletalMeshCompLUT.Contains(ComponentTag))
	{
		USkeletalMeshComponent* PendingComp = *SkeletalMeshCompLUT.Find(ComponentTag);
		if (PendingComp != nullptr && !PendingComp->IsPendingKill()) return PendingComp;
	}

	if (GetOwner() == nullptr) return nullptr;

	TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	GetOwner()->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);

	for (auto SkeletalMeshComp : SkeletalMeshComps)
	{
		if (SkeletalMeshComp != nullptr && SkeletalMeshComp->ComponentHasTag(ComponentTag))
		{
			SkeletalMeshCompLUT.Add(ComponentTag, SkeletalMeshComp);
			return SkeletalMeshComp;
		}
	}

	return nullptr;
}

void UCharacterCustomiserComponent::ApplyBodyMaterialParameters(EBodyMaterialType BodyMaterialTypeToApplyTo, TMap<FName, float> Parameters)
{
	FBodyMaterialPropertiesInternal* BMPI = BodyMaterials.Find(BodyMaterialTypeToApplyTo);

	if (BMPI == nullptr || BMPI->Material == nullptr) return;

	for (auto Parameter : Parameters)
	{
		BMPI->Material->SetScalarParameterValue(Parameter.Key, Parameter.Value);
	}
}

void UCharacterCustomiserComponent::ApplyBodyMaterialParameters(EBodyMaterialType BodyMaterialTypeToApplyTo, TMap<FName, FVector> Parameters)
{
	FBodyMaterialPropertiesInternal* BMPI = BodyMaterials.Find(BodyMaterialTypeToApplyTo);

	if (BMPI == nullptr || BMPI->Material == nullptr) return;
	
	for (auto Parameter : Parameters)
	{
		BMPI->Material->SetVectorParameterValue(Parameter.Key, Parameter.Value);
	}
}

void UCharacterCustomiserComponent::ApplyBodyMaterialParameters(EBodyMaterialType BodyMaterialTypeToApplyTo, TMap<FName, UTexture*> Parameters)
{
	FBodyMaterialPropertiesInternal* BMPI = BodyMaterials.Find(BodyMaterialTypeToApplyTo);

	if (BMPI == nullptr || BMPI->Material == nullptr) return;

	for (auto Parameter : Parameters)
	{
		BMPI->Material->SetTextureParameterValue(Parameter.Key, Parameter.Value);
	}
}

void UCharacterCustomiserComponent::CreateBodyMaterials(bool Force)
{
	USkeletalMeshComponent* MasterMeshComp = FindSkeletalMeshCompByTag(MasterMeshCompName);

	if (MasterMeshComp == nullptr || BodyMaterialProperties.Num() == 0) return;

	if (Force) BodyMaterials.Empty(BodyMaterialProperties.Num());

	if (BodyMaterials.Num() == 0)
	{
		for (auto BodyMaterialProperty : BodyMaterialProperties)
		{
			if (BodyMaterialProperty.Value.Material == nullptr) break;

			if (BodyMaterialProperty.Value.MaterialIndex > MasterMeshComp->GetNumMaterials() - 1) break;

			UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(BodyMaterialProperty.Value.Material, this);
			FBodyMaterialPropertiesInternal NewBMPI;
			NewBMPI.MaterialIndex = BodyMaterialProperty.Value.MaterialIndex;
			NewBMPI.Material = NewMID;
			BodyMaterials.Add(BodyMaterialProperty.Key, NewBMPI);
		}

		for (auto BodyMaterialElem : BodyMaterials)
		{
			MasterMeshComp->SetMaterial(BodyMaterialElem.Value.MaterialIndex, BodyMaterialElem.Value.Material);
		}
	}
}

void UCharacterCustomiserComponent::ApplyMeshPropertiesInternal(USkeletalMeshComponent * SkeletalMeshComponent, FMeshProperties MeshProperties)
{
	USkeletalMeshComponent* MasterMeshComp = FindSkeletalMeshCompByTag(MasterMeshCompName);
	if (MasterMeshComp == nullptr ||SkeletalMeshComponent == nullptr || MeshProperties.Mesh == nullptr) return;

	SkeletalMeshComponent->SetSkeletalMesh(MeshProperties.Mesh);

	if (MeshProperties.Materials.Num() == 0) return;

	for (int32 i = 0; i < MeshProperties.Materials.Num(); i++)
	{
		if (i < SkeletalMeshComponent->GetNumMaterials())
		{
			UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(MeshProperties.Materials[i], this);
			SkeletalMeshComponent->SetMaterial(i, NewMID);

			for (auto ScalarParam : MeshProperties.ScalarParameters)
			{
				NewMID->SetScalarParameterValue(ScalarParam.Key, ScalarParam.Value);
			}

			for (auto VectorParam : MeshProperties.VectorParameters)
			{
				NewMID->SetVectorParameterValue(VectorParam.Key, VectorParam.Value);
			}

			for (auto TextureParam : MeshProperties.TextureParameters)
			{
				NewMID->SetTextureParameterValue(TextureParam.Key, TextureParam.Value);
			}
		}
	}

	SkeletalMeshComponent->SetMasterPoseComponent(MasterMeshComp, true);
	// SkeletalMeshComponent->SetTickGroup(ETickingGroup::TG_PostPhysics);
}

// Copyright Robert Uszynski 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueStructures.generated.h"

/* Structure containing information about dialogues. */
USTRUCT(BlueprintType)
struct FTopDownDialogueProperties : public FTableRowBase
{
	GENERATED_BODY()

public:
	/* Text content of the dialogue. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogueText;

	/* Name of the speaker. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SpeakerName;

	/* Map of RowNames in this datatable and the dialogue which "prompts" the dialogue line. They key MUST match a RowName in this datatable.
	For example, pair might be Name("AskAboutCurio") -> String("That's an interesting object. How much are you selling it for?")
	If this map is empty, it is assumed the line of dialogue is the last line of dialogue. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> BranchToMap;

	/* If this conversation line is reached and this is true, it is assumed that this line is the final line of dialogue, regardless of
	what may be contained in BranchToMap. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLastLineOfConversation;

	/* The static "animation" of the speaker. This is simply a pose baked into a static mesh. The dialogue room will prefer to use a static mesh if one is provided. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* StaticAnimation;

	/* The skeletal mesh representation of the speaker. It will be used if no static mesh is provided, and will use the provided animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* SkeletalMeshRepresentation;

	/* If no static or skeletal meshes are provided, the Dialogue room will instead try to spawn an character from the provided class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACharacter> CharacterSpawnClass;

	/* The animation to use on the skeletal mesh representation of the speaker (or on the spawned character's skeletal mesh). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequenceBase* SkeletalMeshAnimation;

	/* If no animation is provided, the skeletal mesh or the spawned Character's skeletal mesh will use this animation instance. Note that
	individual animations have to be controlled externally. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UAnimInstance> DialogueAnimInstance;

	/* Adjusts the static/skeletal/spawned character RELATIVE to the Dialogue Room. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeAdjustTransform;
};

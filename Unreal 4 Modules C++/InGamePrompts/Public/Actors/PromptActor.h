// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures/IconCreatorSettings.h"
#include "PromptActor.generated.h"

UENUM(BlueprintType)
enum class EPromptProximityType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisplayName = "Sphere")
};

UCLASS()
class INGAMEPROMPTS_API APromptActor : public AActor
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	/* Scene Root */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* SceneRoot;

	/* Prompt becomes visible when player enters this box and if Prompt Proximity Type is Box. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* PromptProximityBox;

	/* Prompt becomes visible when player enters this Sphere and if Prompt Proximity Type is Sphere. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* PromptProximitySphere;

	/* The actual Prompt widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* PromptComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration", meta = (BlueprintBaseOnly))
	TSubclassOf<class UPrompt> PromptWidgetClass;

	/* Name of action event which allows interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration")
	FName InteractKeyActionEventName;

	/* Type of prompt proximity type to use. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration")
	EPromptProximityType PromptProximityType;

	/* Whether the prompt is initially enabled. If false, it has to be enabled via SetPromptEnabled(). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration")
	bool bInitiallyEnabled;

	/* Whether the prompt only displays text (noting it is Rich Text) and no icon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration")
	bool bIsTextOnly;

	/* Text this prompt should display. Note that text is rich text, so you can use text such as:
	"<MyStyle>This sentence uses MyStyle text</>" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration")
	FText PromptText;

	/* Settings used when generating the icon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Actor Configuration", meta = (EditCondition="!bIsTextOnly"))
	FIconCreatorSettings IconCreatorSettings;

private:
	/* Pointer to current prompt widget. */
	class UPrompt* PromptWidget;

	/* Whether or not the player is currently in the proximity radius. */
	bool bPlayerInProximityRadius;

	/* Whether or not prompt is currently enabled. */
	bool bIsPromptEnabled;

public:	
	/**
	 * Constructor.
	 */
	APromptActor();

	/**
	 * Sets prompt enabled or disabled.
	 * @param NewEnabled True to enable, false to disable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Prompt Actor")
	void SetPromptEnabled(bool NewEnabled);

protected:
	/**
	 * Override of construction script to set Box/Sphere properly.
	 * @param Transform Unused.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;

	/**
	 * Begin play to configure collision delegates and other settings.
	 */
	virtual void BeginPlay() override;

private:
	/**
	 * Utility to get the prompt widget from PromptWidgetComponent.
	 * @return Prompt Widget PromptWidgetComponent has PromptWidgetClass, nullptr otherwise.
	 */
	class UPrompt* GetPromptWidget() const;

	/**
	 * Bound to begin collision events of Box/Sphere proximities.
	 */
	UFUNCTION()
	void OnOverlapBeginBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBeginSphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Bound to end collision events of Box/Sphere proximities.
	 */
	UFUNCTION()
	void OnOverlapEndBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnOverlapEndSphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * Internal function called by OnOverlap functions, to test if overlapping actor is player and to
	 * initialise + show the widget if they are.
	 * @param OverlappingActor Actor which overlapped proximity radius.
	 * @param Entered Whether OverlappingActor began the overlap or ended it.
	 */
	void InitialisePrompt(class AActor* OverlappingActor, bool Entered);
};

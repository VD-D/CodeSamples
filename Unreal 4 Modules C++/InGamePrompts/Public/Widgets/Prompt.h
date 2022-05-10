// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structures/IconCreatorSettings.h"
#include "Prompt.generated.h"

/**
 * 
 */
UCLASS()
class INGAMEPROMPTS_API UPrompt : public UUserWidget
{
	GENERATED_BODY()
/* --- VARIABLES --- */
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UIconCreator* IconCreator;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class URichTextBlock* Text;

	/* It is assumed the widget fades in when playing forward. */
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FadeAnimation;

	/* Since UMG does not provide any method to get animation lengths, it must be manually entered here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt")
	float FadeAnimationLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt")
	float FadeAnimationPlaybackSpeed = 1.0f;

private:
	bool bIsFadeAnimationPlaying;

/* --- FUNCTIONS --- */
public:
	UFUNCTION(BlueprintCallable, Category = "Prompt")
	void SetPromptProperties(const FText& NewRichText, bool IsTextOnly, const FIconCreatorSettings& NewIconCreatorSettings);

	UFUNCTION(BlueprintCallable, Category = "Prompt")
	void FadeIn();

	UFUNCTION(BlueprintCallable, Category = "Prompt")
	void FadeOut();

private:
	/**
	 * DEPRECATED
	 * Utility to properly format rich text.
	 * @param String New string to use.
	 */
	// void UpdateRichTextBlock(const FString& String);
};

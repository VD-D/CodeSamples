// Copyright Robert Zygmunt Uszynski 2021-2022

/* Class header */
#include "Widgets/Prompt.h"

/* Other includes. */
#include "Components/RichTextBlock.h"
#include "Widgets/IconCreator.h"

void UPrompt::SetPromptProperties(const FText& NewRichText, bool IsTextOnly, const FIconCreatorSettings& NewIconCreatorSettings)
{
    if (Text != nullptr) Text->SetText(NewRichText);
    if (IconCreator != nullptr && !IsTextOnly) 
    {
        IconCreator->SetParametersFromSettings(NewIconCreatorSettings);
        IconCreator->RebuildIcon();
        IconCreator->SetVisibility(ESlateVisibility::Visible);
    }
    if (IsTextOnly) IconCreator->SetVisibility(ESlateVisibility::Collapsed);
}

void UPrompt::FadeIn()
{
    /* This condition implies the animation is fading out, but we want to fade it back in. */
    if (IsAnimationPlaying(FadeAnimation))
    {
        StopAnimation(FadeAnimation);
        float PlaybackPosition = GetAnimationCurrentTime(FadeAnimation);
        PlayAnimationForward(FadeAnimation);
    }
    /* Otherwise, play from the start. */
    else 
    {
        SetAnimationCurrentTime(FadeAnimation, 0.0f);
        PlayAnimationForward(FadeAnimation, FadeAnimationPlaybackSpeed);
    }
}

void UPrompt::FadeOut()
{
    /* This condition implies the animation is fading in, but we want to fade it back out. */
    if (IsAnimationPlaying(FadeAnimation))  PlayAnimationReverse(FadeAnimation, FadeAnimationPlaybackSpeed);
    /* Otherwise, fade out from the end. */
    else 
    {
        SetAnimationCurrentTime(FadeAnimation, FadeAnimationLength);
        PlayAnimationReverse(FadeAnimation, FadeAnimationPlaybackSpeed);
    }
}

/*
void UPrompt::UpdateRichTextBlock(const FString& String)
{
    if (Text == nullptr) return;

    Text->SetText(FText::Format(NSLOCTEXT("InGamePrompts", "PromptText", "{0}"), String));
}
*/
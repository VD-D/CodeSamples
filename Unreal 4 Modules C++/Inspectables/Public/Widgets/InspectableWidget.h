// Copyright Robert Zygmunt Uszynski 2021-2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InspectableWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInspectableWidgetCloseButtonClicked);

UCLASS()
class INSPECTABLES_API UInspectableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Image to which render targets are assigned to. This has to be done manually as there
	is no way to express this in C++ code. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* RenderImage;

	/* By default removes this widget from the viewport. Can be overriden for custom functionality. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* CloseButton;

	/* Rich text which will have information passed to it from AInGameInspectable. */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	class URichTextBlock* TextBlock;

	UPROPERTY(BlueprintAssignable, Category = "Inspectable Widget")
	FInspectableWidgetCloseButtonClicked InspectableWidgetCloseButtonClicked;

	/**
	 * Function called by CloseButton.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inspectable Widget")
	void CloseButtonPressed();
	virtual void CloseButtonPressed_Implementation();

	/**
	 * Updates TextBlock's text. Note this is rich text, so HTML tags like <MyStyle> My Style text </> are permitted. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Inspectable Widget")
	void UpdateText(const FText& NewText);

protected:
	/**
	 * Binds CloseButton delegate.
	 */
	virtual void NativeConstruct() override;

private:
	/**
	 * Also bound to CloseButton clicked. Simply broadcasts InspectableWidgetCloseButtonClicked.
	 */
	UFUNCTION()
	void BroadcastClosedDelegate();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"
#include "RichTextBlockInlineWidget.generated.h"

class ISlateStyle;

/**
 * Struct defining a widget class.
 */
USTRUCT(Blueprintable, BlueprintType)
struct AUTOBATTLERPLUGIN_API FRichInlineWidgetClass : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Widget)
	TSubclassOf<UUserWidget> WidgetClass;
};

/**
 * Creates arbitary inline widget as a decorator.
 * 
 * Understands the format <widget src="NameOfWidgetClassInDatatable" args="arg1,arg2">
 * SEE: IInlineWidgetListener
 */
UCLASS(Abstract, Blueprintable)
class AUTOBATTLERPLUGIN_API URichTextBlockInlineWidget : public URichTextBlockDecorator
{
	GENERATED_BODY()
/////////////////////////////////////////////////////////////////////////////////
//// DECORATOR API
/////////////////////////////////////////////////////////////////////////////////
public:
	/**
	 * Default constructor.
	 */
	URichTextBlockInlineWidget(const FObjectInitializer& ObjectInitializer);

	/**
	 * Slate API. Generates actual decorator widget.
	 * @param InOwner Rich Text block which owns the decorator.
	 */
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	/**
	 * Utility to get WidgetClass from decorator datatable.
	 * @param RowName Row name of in datatable.
	 * @return Found row or nullptr if not found.
	 */
	FRichInlineWidgetClass* FindWidgetClass(const FName& RowName);

	/* All widget classes creatable by this decorator. */
	UPROPERTY(EditAnywhere, Category = "Widget", meta = (RequiredAssetDataTags = "RowStructure=FRichInlineWidgetClass"))
	UDataTable* ClassDatatable;
};

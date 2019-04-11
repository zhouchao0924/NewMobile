// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DRPanel.h"
#include "Blueprint/UserWidget.h"
#include "DRMainFrame.generated.h"

class UDREditorViewportWidget;

UCLASS(BlueprintType, abstract)
class UDRMainFrame : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	bool Initialize() override;

	UFUNCTION(BlueprintPure)
	UDREditorViewportWidget *GetViewportWidget() { return ViewportWidget; }
	
	UFUNCTION(BlueprintCallable)
	UDRPanel *GetOperationPanel() { return OperationPanel; }
protected:
	UPROPERTY(Transient)
	UDREditorViewportWidget *ViewportWidget;

	UPROPERTY(Transient)
	UDRPanel				*OperationPanel;

	UPROPERTY(Transient)
	UDRPanel				*PropertyPanel;
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "DRPanel.generated.h"

UCLASS(BlueprintType)
class UDRPanel : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public: 
	bool Initialize() override;
	void SetWidget(UWidget *SlotWidget);
protected:
	UPROPERTY(EditAnywhere)
	FString		 WidgetSlotName;
	UPROPERTY(Transient)
	UWidget		 *Widget;
	UPROPERTY(Transient)
	UPanelWidget *WidgetContainer;
};



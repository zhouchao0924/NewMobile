// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "DRPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Components/PanelWidget.h"

UDRPanel::UDRPanel(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UDRPanel::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	WidgetContainer = Cast<UPanelWidget>(GetWidgetFromName(FName(*WidgetSlotName)));
	
	return true;
}

void UDRPanel::SetWidget(UWidget *SlotWidget)
{
	if (WidgetContainer)
	{
		WidgetContainer->ClearChildren();
		if (SlotWidget)
		{
			WidgetContainer->AddChild(SlotWidget);
		}
	}
}



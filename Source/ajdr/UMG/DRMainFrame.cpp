// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "DRMainFrame.h"
#include "Components/Border.h"
#include "DREditorViewportWidget.h"


UDRMainFrame::UDRMainFrame(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UDRMainFrame::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	ViewportWidget = Cast<UDREditorViewportWidget>(GetWidgetFromName(FName("ViewportWidgetUMG")));
	OperationPanel = Cast<UDRPanel>(GetWidgetFromName(FName("UMG_OperationPanel")));
	PropertyPanel = Cast<UDRPanel>(GetWidgetFromName(FName("UMG_PropertyPanel")));

	return true;
}




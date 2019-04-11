// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "PreviewScene.h"
#include "DRLevelScene.h"
#include "DREditorViewportWidget.h"
#include "DREditorViewportClient.h"
#include "Components/PrimitiveComponent.h"

#define LOCTEXT_NAMESPACE "DR"

void SDRViewport::Construct(const FArguments& InArgs)
{
	LevelName = InArgs._LevelName;

	if (LevelName.Len() > 0)
	{
		PreviewScene = new FDRLevelScene(LevelName);
	}
	else
	{
		PreviewScene = new FDRPreviewScene();
	}

	if (PreviewScene)
	{
		PreviewScene->ConstructWorld();
	}

	SViewport::FArguments ParentArgs;
	ParentArgs.IgnoreTextureAlpha(false);
	ParentArgs.EnableBlending(false);
	SViewport::Construct(ParentArgs);
	
	ViewportClient = MakeShareable(new FDREditorViewportClient(PreviewScene, SharedThis(this)));
	Viewport = MakeShareable(new FDRSceneViewport(ViewportClient.Get(), SharedThis(this)));
	ViewportClient->Viewport = Viewport->GetViewport();

	// The viewport widget needs an interface so it knows what should render
	SetViewportInterface(Viewport.ToSharedRef());
}

void SDRViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
 	Viewport->Invalidate();
	Viewport->Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	ViewportClient->Tick(InDeltaTime);
}

FDREditorViewportClient *SDRViewport::GetViewportClient()
{
	return ViewportClient.Get();
}

UDREditorViewportWidget::UDREditorViewportWidget(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UDREditorViewportWidget::SetCameraMod(ECameraMod Mod)
{
	FDREditorViewportClient *ViewportClient = ViewportWidget->GetViewportClient();
	if (ViewportClient)
	{
		ViewportClient->SetCameraMod(Mod);
	}
}

TSharedRef<SWidget> UDREditorViewportWidget::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("DREditorViewportWidget", "DREditorViewportWidget"))
			];
	}
	else
	{
		ViewportWidget = SNew(SDRViewport)
						.LevelName(PreviewLevelName);

		if (GetChildrenCount() > 0)
		{
			ViewportWidget->SetContent(GetContentSlot()->Content ? GetContentSlot()->Content->TakeWidget() : SNullWidget::NullWidget);
		}

		FDREditorViewportClient *ViewportClient = ViewportWidget->GetViewportClient();
		if (ViewportClient)
		{
			ViewportClient->OnActorSelectChangedCallback.BindUObject(this, &UDREditorViewportWidget::OnViewportActorSelectedChanged);
			ViewportClient->OnBuildingSelectChangedCallback.BindUObject(this, &UDREditorViewportWidget::OnViewportBuildingSelectedChanged);
		}

		return ViewportWidget.ToSharedRef();
	}
}

void UDREditorViewportWidget::OnViewportActorSelectedChanged(HActor *HitActor)
{
	if (HitActor)
	{
		AActor *SelectedActor = HitActor->Actor;
		UE_LOG(LogDR, Log, TEXT("Selected Actor %s, MaterialIndex %d"), *SelectedActor->GetName(), HitActor->MaterialIndex);
		UPrimitiveComponent *PrimComponent = const_cast<UPrimitiveComponent *>(HitActor->PrimComponent);
		if (ViewportActorSelectedChanged.IsBound())
		{
			ViewportActorSelectedChanged.Broadcast(SelectedActor, PrimComponent, HitActor->SectionIndex, HitActor->MaterialIndex);
		}
	}
}

void UDREditorViewportWidget::OnViewportBuildingSelectedChanged(HBuildingObject *HitBuilding)
{
	if (HitBuilding)
	{
		UE_LOG(LogDR, Log, TEXT("Selected Building %d"), HitBuilding->BuildingID);
		if (ViewportBuildingSelectedChanged.IsBound())
		{
			ViewportBuildingSelectedChanged.Broadcast(HitBuilding->BuildingID);
		}
	}
}

FDREditorViewportClient *UDREditorViewportWidget::GetViewportClient()
{
	return ViewportWidget.IsValid()? ViewportWidget->GetViewportClient() : nullptr;
}

TSharedRef<SDRViewport> UDREditorViewportWidget::GetViewportWidget()
{
	return ViewportWidget.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraMod.h"
#include "DREditorViewportClient.h"
#include "Components/ContentWidget.h"
#include "DREditorViewportWidget.generated.h"

class SDRViewport : public SViewport
{
	SLATE_BEGIN_ARGS(SDRViewport)
	{}
	SLATE_ARGUMENT(FString, LevelName)
	SLATE_END_ARGS()

	SDRViewport()
	:PreviewScene(nullptr){}

	void Construct(const FArguments& InArgs);
	FDREditorViewportClient *GetViewportClient();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
public:
	TSharedPtr<FDREditorViewportClient>	ViewportClient;
	TSharedPtr<FSceneViewport>			Viewport;
	FDRPreviewScene						*PreviewScene;
	FString								LevelName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FViewportActorSelectChanged, AActor *, SelectedActor, UPrimitiveComponent *, PrimComponent, int32, SectionIndex, int32, MaterialIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FViewportBuildingSelectChanged, int32, BuildingID);

UCLASS(BlueprintType)
class UDREditorViewportWidget :public UContentWidget
{

	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetCameraMod(ECameraMod Mod);
	TSharedRef<SWidget> RebuildWidget() override;
	FDREditorViewportClient *GetViewportClient();
	TSharedRef<SDRViewport> GetViewportWidget();
protected:
	void OnViewportActorSelectedChanged(HActor *HitActor);
	void OnViewportBuildingSelectedChanged(HBuildingObject *HitBuilding);
protected:
	TSharedPtr<SDRViewport>	ViewportWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString					PreviewLevelName;
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FViewportActorSelectChanged	ViewportActorSelectedChanged;
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FViewportBuildingSelectChanged ViewportBuildingSelectedChanged;
};


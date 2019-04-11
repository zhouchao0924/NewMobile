// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"
#include "UnrealClient.h"
#include "Tickable.h"
#include "DRPreviewScene.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "Delegates/DelegateCombinations.h"
#include "GenericPlatform/ICursor.h"
#include "Camera/CameraActor.h"
#include "DRHitProxy.h"

class FDrawer;
class SDRViewport;
class FDRSceneViewport;

class FDREditorViewportClient : public FCommonViewportClient, public FViewElementDrawer, public FGCObject
{
public:
	DECLARE_DELEGATE_OneParam(FOnDraw, FDrawer *);
	DECLARE_DELEGATE_OneParam(FOnActorSelectChanged, HActor *);
	DECLARE_DELEGATE_OneParam(FOnBuildingSelectChanged, HBuildingObject *);
	DECLARE_MULTICAST_DELEGATE_FourParams(FOnInputAxis, FViewport *, FKey, float, float);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInputKey, FViewport *, FKey, EInputEvent);

	FDREditorViewportClient();
	FDREditorViewportClient(FDRPreviewScene *InPreviewScene, TSharedPtr<SDRViewport> InViewportSlate);
	virtual ~FDREditorViewportClient();
	
	virtual void Tick(float DeltaTime);
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual UWorld* GetPreviewWorld() const;
	virtual FSceneView* GetView(FSceneViewFamily* ViewFamily, int32 SizeX, int32 SizeY, const FLinearColor &BackgroundColor, const EStereoscopicPass StereoPass);
	virtual FSceneInterface* GetScene() const;
	virtual FLinearColor GetBackgroundColor() const;

	float GetNearClipPlane() const;
	float GetFarClipPlaneOverride() const;
	void DeprojectScreenToWorld(const FVector2D& ScreenPos, FVector& out_WorldOrigin, FVector& out_WorldDirection);
	FVector2D ProjectWorldToScreen(const FVector& WorldPosition);
	FVector2D GetCursorPosition();
	FVector GetWorldPositionAtCursor(float Dist = 0);
	void SetCameraMod(ECameraMod CameraMod);

	bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = false) override;
	struct FEngineShowFlags* GetEngineShowFlags() { return &EngineShowFlags; }

	void  SetLight(bool bInLighting);
	float GetHitProxySize();
	void  AddReferencedObjects(FReferenceCollector& Collector) override;
	bool  IsKeyDown(FKey Key);
	void  SetWireframe(bool bWireframe);
	void  SetBuildingActorVisibleOnly(bool bBuildingActorVisibleOnly);
	void  SetCursor(EMouseCursor::Type Cursor);
	SDRViewport *GetViewportSlate() { return ViewportSlate.Pin().Get(); }
	ASceneManager *GetSceneManager();
	void UpdateProjectView();
	bool HitTestByCursor(int32 X, int32 Y);
private:
	bool IsVisible() const { return true; }
	void FlushLineBatcher();
	FMatrix CalculateProjectionMatrix(const FMinimalViewInfo& MinimalView);
public:
	FOnDraw							OnDrawCallback;
	FOnInputAxis					OnInputAxisCallback;
	FOnInputKey						OnInputKeyCallback;
	FOnActorSelectChanged			OnActorSelectChangedCallback;
	FOnBuildingSelectChanged		OnBuildingSelectChangedCallback;
	FDRPreviewScene					*PreviewScene;
	FEngineShowFlags				EngineShowFlags;
	FViewport						*Viewport;
	TSharedPtr<FDRSceneViewport>	ScenceViewport;
	FSceneViewStateReference		ViewState;
	FCameraManager					CameraMgr;
	TWeakPtr<SDRViewport>			ViewportSlate;
	TWeakObjectPtr<ASceneManager>	SceneManager;
	FMatrix							ViewRotationMatrix;
	FMatrix							ProjectionMatrix;
	FMatrix							ViewProjectMatrix;
	FMatrix							InvViewProjectMatrix;
	bool							bBuildingActorVisibleOnly;
	int32							HitProxySize;
	FDrawer							*Drawer;
};

class FDRSceneViewport : public FSceneViewport
{
public:
	FDRSceneViewport(FViewportClient* InViewportClient, TSharedPtr<SViewport> InViewportWidget);
	float GetDisplayGamma() const override;
	void SetGamma(float InGamma);
private:
	float GammaAdjust;
};



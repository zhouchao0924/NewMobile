
#include "ajdr.h"
#include "DREditorViewportClient.h"
#include "HighResScreenshot.h"
#include "SceneViewExtension.h"
#include "CanvasTypes.h"
#include "EngineModule.h"
#include "Stats/Stats.h"
#include "Stats/Stats2.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Actor/DRActor.h"
#include "DRDrawer.h"
#include "DREditorViewportWidget.h"
#include "DRHitProxy.h"
#include "LegacyScreenPercentageDriver.h"

DEFINE_LOG_CATEGORY(LogDR);

FDREditorViewportClient::FDREditorViewportClient()
	: FCommonViewportClient()
	, PreviewScene(nullptr)
	, EngineShowFlags(ESFIM_Game)
	, HitProxySize(5)
	, Drawer(nullptr)
{
}

FDREditorViewportClient::FDREditorViewportClient(FDRPreviewScene *InPreviewScene, TSharedPtr<SDRViewport> InViewportSlate)
	: FDREditorViewportClient()
{
	PreviewScene = InPreviewScene;
	ViewportSlate = InViewportSlate;

	EngineShowFlags.MotionBlur = 0;
	EngineShowFlags.SetGame(true);
	EngineShowFlags.SetBloom(false);
	EngineShowFlags.SetVisualizeHDR(false);
	EngineShowFlags.SetSnap(1);
	EngineShowFlags.SetAntiAliasing(true);
	ViewState.Allocate();

	CameraMgr.SetViewportClient(this);

	UWorld *MyWorld = PreviewScene->GetWorld();
	if (MyWorld)
	{
		SceneManager = MyWorld->SpawnActor<ASceneManager>(ASceneManager::StaticClass());
		for (TActorIterator<ADRActor> It(MyWorld); It; ++It)
		{
			ADRActor *pActor = *It;
			if (pActor)
			{
				SceneManager->AddActor(pActor);
			}
		}
	}

	Drawer = new FDrawer(GMaxRHIFeatureLevel);
	Drawer->SetViewIndex(0);
}

FDREditorViewportClient::~FDREditorViewportClient()
{
	if (Drawer)
	{
		delete Drawer;
		Drawer = nullptr;
	}
}

ASceneManager *FDREditorViewportClient::GetSceneManager()
{ 
	return SceneManager.Get();
}

UWorld* FDREditorViewportClient::GetPreviewWorld() const
{
	return PreviewScene ? PreviewScene->GetWorld() : NULL;
}

FSceneInterface* FDREditorViewportClient::GetScene() const
{
	return PreviewScene ? PreviewScene->GetScene() : NULL;
}

float FDREditorViewportClient::GetFarClipPlaneOverride() const
{
	return 9000000.0f;
}

float FDREditorViewportClient::GetNearClipPlane() const
{
	return GNearClippingPlane;
}

FLinearColor FDREditorViewportClient::GetBackgroundColor() const
{
	FLinearColor BackgroundColor = EngineShowFlags.Wireframe? FColor::Black : FColor(55, 55, 55);
	return BackgroundColor;
}

void FDREditorViewportClient::SetLight(bool bInLighting)
{
	EngineShowFlags.SetLighting(bInLighting);
}

/*
 * Screen percentage interface that is just constantly changing res to test resolution changes.
 */
class FDRScreenPercentageHellDriver : public ISceneViewFamilyScreenPercentage
{
public:

	FDRScreenPercentageHellDriver(const FSceneViewFamily& InViewFamily)
		: ViewFamily(InViewFamily)
	{ }

	virtual float GetPrimaryResolutionFractionUpperBound() const override
	{
		return 1.0f;
	}

	virtual ISceneViewFamilyScreenPercentage* Fork_GameThread(const class FSceneViewFamily& ForkedViewFamily) const override
	{
		check(IsInGameThread());

		if (ForkedViewFamily.Views[0]->State)
		{
			return new FDRScreenPercentageHellDriver(ForkedViewFamily);
		}

		return new FLegacyScreenPercentageDriver(
			ForkedViewFamily, /* GlobalResolutionFraction = */ 1.0f, /* AllowPostProcessSettingsScreenPercentage = */ false);
	}

	virtual void ComputePrimaryResolutionFractions_RenderThread(TArray<FSceneViewScreenPercentageConfig>& OutViewScreenPercentageConfigs) const override
	{
		check(IsInRenderingThread());

		// Early return if no screen percentage should be done.
		if (!ViewFamily.EngineShowFlags.ScreenPercentage)
		{
			return;
		}

		uint32 FrameId = ViewFamily.Views[0]->State->GetFrameIndexMod8();
		float ResolutionFraction = FrameId == 0 ? 1.f : (FMath::Cos((FrameId + 0.25) * PI / 8) * 0.25f + 0.75f);

		for (int32 i = 0; i < ViewFamily.Views.Num(); i++)
		{
			OutViewScreenPercentageConfigs[i].PrimaryResolutionFraction = ResolutionFraction;
		}
	}

private:
	// View family to take care of.
	const FSceneViewFamily& ViewFamily;
};

void FDREditorViewportClient::Draw(FViewport* InViewport, FCanvas* Canvas)
{
	float TimeSeconds;
	float RealTimeSeconds;
	float DeltaTimeSeconds;

	UWorld* World = GetPreviewWorld();
	// Use time relative to start time to avoid issues with float vs double
	TimeSeconds = FApp::GetCurrentTime() - GStartTime;
	RealTimeSeconds = FApp::GetCurrentTime() - GStartTime;
	DeltaTimeSeconds = FApp::GetDeltaTime();

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Canvas->GetRenderTarget(),
		GetScene(),
		EngineShowFlags)
		.SetWorldTimes(TimeSeconds, DeltaTimeSeconds, RealTimeSeconds)
		.SetRealtimeUpdate(true)
	);

	ISceneViewFamilyScreenPercentage *ScreenPercentageInterface = new FLegacyScreenPercentageDriver(ViewFamily, 1.0f, /* AllowPostProcessSettingsScreenPercentage = */ false);
	ViewFamily.SetScreenPercentageInterface(ScreenPercentageInterface);

	Canvas->SetScaledToRenderTarget(false);
	Canvas->SetStereoRendering(false);

	EngineShowFlagOverride(ESFIM_Game, VMI_Lit, ViewFamily.EngineShowFlags, NAME_None);

	const FIntPoint ViewportSizeXY = Viewport->GetSizeXY();
	FSceneView *View = GetView(&ViewFamily, ViewportSizeXY.X, ViewportSizeXY.Y, GetBackgroundColor(), eSSP_FULL);
	View->CameraConstrainedViewRect = View->UnscaledViewRect;

	ViewFamily.Views.Add(View);
	for (auto ViewExt : ViewFamily.ViewExtensions)
	{
		ViewExt->SetupView(ViewFamily, *View);
	}
	
	Canvas->Clear(FLinearColor::Black);

// 	ViewFamily.EngineShowFlags.ScreenPercentage = true;
// 	ViewFamily.SetScreenPercentageInterface(new FDRScreenPercentageHellDriver(ViewFamily));
	GetRendererModule().BeginRenderingViewFamily(Canvas, &ViewFamily);
}

void FDREditorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (PDI)
	{
		Drawer->SetPDI(PDI);
		Drawer->SetCanvas(nullptr);
		OnDrawCallback.ExecuteIfBound(Drawer);
	}
}

FMatrix FDREditorViewportClient::CalculateProjectionMatrix(const FMinimalViewInfo& MinimalView)
{
	FMatrix CaclProjectionMatrix;

	if (MinimalView.ProjectionMode == ECameraProjectionMode::Orthographic)
	{
		const float YScale = 1.0f / MinimalView.AspectRatio;

		const float HalfOrthoWidth = MinimalView.OrthoWidth / 2.0f;
		const float ScaledOrthoHeight = MinimalView.OrthoWidth / 2.0f * YScale;

		const float NearPlane = MinimalView.OrthoNearClipPlane;
		const float FarPlane = MinimalView.OrthoFarClipPlane;

		const float ZScale = 1.0f / (FarPlane - NearPlane);
		const float ZOffset = -NearPlane;

		CaclProjectionMatrix = FReversedZOrthoMatrix(
			HalfOrthoWidth,
			ScaledOrthoHeight,
			ZScale,
			ZOffset
		);
	}
	else
	{
		// Avoid divide by zero in the projection matrix calculation by clamping FOV
		CaclProjectionMatrix = FReversedZPerspectiveMatrix(
			FMath::Max(0.001f, MinimalView.FOV) * (float)PI / 360.0f,
			MinimalView.AspectRatio,
			1.0f,
			GNearClippingPlane);
	}

	if (!MinimalView.OffCenterProjectionOffset.IsZero())
	{
		const float Left = -1.0f + MinimalView.OffCenterProjectionOffset.X;
		const float Right = Left + 2.0f;
		const float Bottom = -1.0f + MinimalView.OffCenterProjectionOffset.Y;
		const float Top = Bottom + 2.0f;
		CaclProjectionMatrix.M[2][0] = (Left + Right) / (Left - Right);
		CaclProjectionMatrix.M[2][1] = (Bottom + Top) / (Bottom - Top);
	}

	return CaclProjectionMatrix;
}

void FDREditorViewportClient::UpdateProjectView()
{
	FIntPoint Size = Viewport->GetSizeXY();

	FMinimalViewInfo MinimalViewInfo;
	CameraMgr.GetMinimalViewInfo(MinimalViewInfo);

	MinimalViewInfo.AspectRatio = float(Size.X) / float(Size.Y);
	MinimalViewInfo.OrthoWidth = Size.X;

	ViewRotationMatrix = FScaleMatrix(FVector(CameraMgr.GetOrthoScale())) * FInverseRotationMatrix(MinimalViewInfo.Rotation) * FMatrix(FPlane(0, 0, 1, 0), FPlane(1, 0, 0, 0), FPlane(0, 1, 0, 0), FPlane(0, 0, 0, 1));
	ProjectionMatrix = CalculateProjectionMatrix(MinimalViewInfo);

	ViewProjectMatrix = FTranslationMatrix(-MinimalViewInfo.Location)*ViewRotationMatrix*ProjectionMatrix;
	InvViewProjectMatrix = ViewProjectMatrix.Inverse();
}

FSceneView* FDREditorViewportClient::GetView(FSceneViewFamily* ViewFamily, int32 SizeX, int32 SizeY, const FLinearColor &BackgroundColor, const EStereoscopicPass StereoPass)
{
	FSceneViewInitOptions ViewInitOptions;

	FMinimalViewInfo MinimalViewInfo;
	CameraMgr.GetMinimalViewInfo(MinimalViewInfo);

	UpdateProjectView();

	AWorldSettings* WorldSettings = nullptr;
	if (GetScene() != nullptr && GetScene()->GetWorld() != nullptr)
	{
		WorldSettings = GetScene()->GetWorld()->GetWorldSettings();
	}

	if (WorldSettings != nullptr)
	{
		ViewInitOptions.WorldToMetersScale = WorldSettings->WorldToMeters;
	}

	FVector		ViewLocation = MinimalViewInfo.Location;
	FRotator	ViewRotation = MinimalViewInfo.Rotation;
	FIntRect	ViewRect = FIntRect(0, 0, SizeX, SizeY);

	ViewInitOptions.SetViewRectangle(ViewRect);
	ViewInitOptions.ViewOrigin = ViewLocation;
	ViewInitOptions.ViewFamily = ViewFamily;
	ViewInitOptions.SceneViewStateInterface = ViewState.GetReference();
	ViewInitOptions.StereoPass = StereoPass;
	ViewInitOptions.ViewElementDrawer = this;
	ViewInitOptions.BackgroundColor = BackgroundColor;
	ViewInitOptions.ViewRotationMatrix = ViewRotationMatrix;
	ViewInitOptions.ProjectionMatrix = ProjectionMatrix;

	EngineShowFlagOrthographicOverride(ViewInitOptions.IsPerspectiveProjection(), ViewFamily->EngineShowFlags);

	FSceneView* View = new FSceneView(ViewInitOptions);
	View->ViewLocation = ViewLocation;
	View->ViewRotation = ViewRotation;

	View->StartFinalPostprocessSettings(View->ViewLocation);
	if (PreviewScene)
	{
		PreviewScene->OverridePostProcessSettings(View);
	}
	View->EndFinalPostprocessSettings(ViewInitOptions);

	FlushLineBatcher();

	return View;
}

void FDREditorViewportClient::FlushLineBatcher()
{
	UWorld *MyWorld = GetPreviewWorld();
	if (MyWorld)
	{
		if (MyWorld->LineBatcher != nullptr)
		{
			MyWorld->LineBatcher->Flush();
		}

		if (MyWorld->ForegroundLineBatcher != nullptr)
		{
			MyWorld->ForegroundLineBatcher->Flush();
		}
	}
}

FVector2D FDREditorViewportClient::GetCursorPosition()
{
	FIntPoint CurPos;
	Viewport->GetMousePos(CurPos);
	return FVector2D(CurPos.X, CurPos.Y);
}

FVector FDREditorViewportClient::GetWorldPositionAtCursor(float Dist /*= 0*/)
{
	FIntPoint CurPos;
	Viewport->GetMousePos(CurPos);
	FVector WorldPos, WorldDirection;
	DeprojectScreenToWorld(CurPos, WorldPos, WorldDirection);
	return WorldPos + WorldDirection*Dist;
}

void FDREditorViewportClient::DeprojectScreenToWorld(const FVector2D& ScreenPos, FVector& out_WorldOrigin, FVector& out_WorldDirection)
{
	const FIntPoint ViewportSizeXY = Viewport->GetSizeXY();
	FSceneView::DeprojectScreenToWorld(ScreenPos, FIntRect(0, 0, ViewportSizeXY.X, ViewportSizeXY.Y), /*FScaleMatrix(1.0f / CameraMgr.GetOrthoScale())**/InvViewProjectMatrix, out_WorldOrigin, out_WorldDirection);
}

FVector2D FDREditorViewportClient::ProjectWorldToScreen(const FVector& WorldPosition)
{
	FVector2D ScreenPos;
	const FIntPoint ViewportSizeXY = Viewport->GetSizeXY();
	FSceneView::ProjectWorldToScreen(WorldPosition, FIntRect(0, 0, ViewportSizeXY.X, ViewportSizeXY.Y), ViewProjectMatrix, ScreenPos);
	return ScreenPos;
}

void FDREditorViewportClient::Tick(float DeltaTime)
{
	UWorld *MyWorld = PreviewScene ? PreviewScene->GetWorld() : NULL;
	if (MyWorld)
	{
		USkyLightComponent::UpdateSkyCaptureContents(MyWorld);
		MyWorld->Tick(LEVELTICK_All, DeltaTime);
	}
}

void FDREditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (ViewState.GetReference())
	{
		ViewState.GetReference()->AddReferencedObjects(Collector);
	}
}

bool FDREditorViewportClient::InputAxis(FViewport* InViewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples/* = 1*/, bool bGamepad /*= false*/)
{
	FCameraMod *CameraMod = CameraMgr.GetCurrentMod();
	if (CameraMod && CameraMod->InputAxis(Key, Delta, DeltaTime))
	{
		return true;
	}

	if (OnInputAxisCallback.IsBound())
	{
		OnInputAxisCallback.Broadcast(InViewport, Key, Delta, DeltaTime);
	}
	
	return true;
}

bool FDREditorViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed /*= 1.f*/, bool bGamepad /*= false*/)
{
	FCameraMod *CameraMod = CameraMgr.GetCurrentMod();
	if (CameraMod && CameraMod->InputKey(Key, Event))
	{
		return true;
	}

	if (OnInputKeyCallback.IsBound())
	{
		OnInputKeyCallback.Broadcast(InViewport, Key, Event);
	}

	return false;
}

bool FDREditorViewportClient::HitTestByCursor(int32 X, int32 Y)
{
	HHitProxy* HitProxy = nullptr;

	int32	MinX = X - HitProxySize;
	int32	MinY = Y - HitProxySize;
	int32	MaxX = X + HitProxySize;
	int32	MaxY = Y + HitProxySize;

	FIntPoint VPSize = Viewport->GetSizeXY();
	MinX = FMath::Clamp(MinX, 0, VPSize.X - 1);
	MinY = FMath::Clamp(MinY, 0, VPSize.Y - 1);
	MaxX = FMath::Clamp(MaxX, 0, VPSize.X - 1);
	MaxY = FMath::Clamp(MaxY, 0, VPSize.Y - 1);

	int32 TestSizeX = MaxX - MinX + 1;
	int32 TestSizeY = MaxY - MinY + 1;

	TArray<HHitProxy*>	ProxyMap;
	Viewport->GetHitProxyMap(FIntRect(MinX, MinY, MaxX+1, MaxY+1), ProxyMap);

	if (ProxyMap.Num() > 0)
	{
		int32 ProxyIndex = TestSizeY / 2 * TestSizeX + TestSizeX / 2;
		check(ProxyIndex < ProxyMap.Num());
		HitProxy = ProxyMap[ProxyIndex];

		bool bIsOrtho = !CameraMgr.IsProjView();
		for (int32 TestY = 0; TestY < TestSizeY; TestY++)
		{
			for (int32 TestX = 0; TestX < TestSizeX; TestX++)
			{
				HHitProxy* TestProxy = ProxyMap[TestY * TestSizeX + TestX];
				if (TestProxy)
				{
					if (TestProxy->IsA(HActor::StaticGetType()))
					{
						HActor *HitActor = (HActor *)TestProxy;
						if (HitProxy && HitActor->MaterialIndex > 0)
						{
							HitProxy = TestProxy;
							break;
						}
						else
						{
							HitProxy = TestProxy;
						}
					}
					else
					{
						HitProxy = TestProxy;
					}
				}
			}
		}
	}

	if (HitProxy)
	{
		if (HitProxy->IsA(HActor::StaticGetType()))
		{
			OnActorSelectChangedCallback.ExecuteIfBound((HActor *)HitProxy);
		}
		else if (HitProxy->IsA(HBuildingObject::StaticGetType()))
		{
			OnBuildingSelectChangedCallback.ExecuteIfBound((HBuildingObject *)HitProxy);
		}
	}

	return HitProxy != nullptr;
}

bool FDREditorViewportClient::IsKeyDown(FKey Key)
{
	return Viewport->KeyState(Key);
}

void FDREditorViewportClient::SetCameraMod(ECameraMod CameraMod)
{
	CameraMgr.SetCameraMod(CameraMod);
}

void FDREditorViewportClient::SetWireframe(bool bWireframe)
{ 
	EngineShowFlags.SetWireframe(bWireframe); 
}

void  FDREditorViewportClient::SetBuildingActorVisibleOnly(bool bInBuildingActorVisibleOnly)
{
	if (bInBuildingActorVisibleOnly != bBuildingActorVisibleOnly)
	{
		bBuildingActorVisibleOnly = bInBuildingActorVisibleOnly;
		if (PreviewScene)
		{
			PreviewScene->SetActorVisible(ADRActor::StaticClass(), bBuildingActorVisibleOnly);
		}
	}
}

void  FDREditorViewportClient::SetCursor(EMouseCursor::Type Cursor)
{
	if (ViewportSlate.IsValid())
	{
		ViewportSlate.Pin()->SetCursor(Cursor);
	}
}

float FDREditorViewportClient::GetHitProxySize()
{ 
	return HitProxySize / CameraMgr.GetOrthoScale();
}

//////////////////////////////////////////////////////////////////////////
FDRSceneViewport::FDRSceneViewport(FViewportClient *InViewportClient, TSharedPtr<SViewport> InViewportWidget)
	:FSceneViewport(InViewportClient, InViewportWidget)
	, GammaAdjust(1.5f)
{
}

void FDRSceneViewport::SetGamma(float InGamma)
{
	GammaAdjust = InGamma;
}

float FDRSceneViewport::GetDisplayGamma() const
{
	return GammaAdjust;
}


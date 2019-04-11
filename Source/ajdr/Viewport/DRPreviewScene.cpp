
#include "ajdr.h"
#include "DRPreviewScene.h"
#include "EngineModule.h"

FDRPreviewScene::FDRPreviewScene(const FDRPreviewScene::ConstructionValues &InCVS)
	: PreviewWorld(NULL)
	, bForceAllUsedMipsResident(InCVS.bForceMipsResident)
	, PreviewActor(NULL)
	, LineBatcher(NULL)
	, DirectionalLight(NULL)
	, Floor(NULL)
	, SkySphere(NULL)
	, SkyLight(NULL)
{
	CVS = InCVS;
	CVS.bEditor = false;
	bInit = false;
}

FDRPreviewScene::~FDRPreviewScene()
{
	Clean();
}

void FDRPreviewScene::Clean()
{
	if (bInit)
	{
		if (PreviewActor)
		{
			PreviewActor->ConditionalBeginDestroy();
			PreviewActor = NULL;
		}

		// Remove all the attached components
		for (int32 ComponentIndex = 0; ComponentIndex < Components.Num(); ComponentIndex++)
		{
			UActorComponent* Component = Components[ComponentIndex];
			if (bForceAllUsedMipsResident)
			{
				UMeshComponent* pMesh = Cast<UMeshComponent>(Component);
				if (pMesh != NULL)
				{
					pMesh->SetTextureForceResidentFlag(false);
				}
			}
			Component->UnregisterComponent();
		}

		if (PreviewWorld)
		{
			PreviewWorld->CleanupWorld();
			GEngine->DestroyWorldContext(GetWorld());
		}

		bInit = false;
	}
}

void FDRPreviewScene::ConstructWorld()
{
	if (!bInit)
	{
		bInit = true;
		PreviewWorld = NewObject<UWorld>();
		PreviewWorld->WorldType = EWorldType::GamePreview;

		if (CVS.bTransactional)
		{
			PreviewWorld->SetFlags(RF_Transactional);
		}

		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(PreviewWorld->WorldType);
		WorldContext.SetCurrentWorld(PreviewWorld);

		PreviewWorld->InitializeNewWorld(UWorld::InitializationValues()
			.AllowAudioPlayback(CVS.bAllowAudioPlayback)
			.CreatePhysicsScene(CVS.bCreatePhysicsScene)
			.RequiresHitProxies(true)
			.CreateNavigation(false)
			.CreateAISystem(false)
			.ShouldSimulatePhysics(CVS.bShouldSimulatePhysics)
			.SetTransactional(CVS.bTransactional));

		PreviewWorld->InitializeActorsForPlay(FURL());

		DirectionalLight = NewObject<UDirectionalLightComponent>(GetTransientPackage());
		DirectionalLight->Intensity = CVS.LightBrightness;
		DirectionalLight->LightColor = FColor::White;
		AddComponent(DirectionalLight, FTransform(CVS.LightRotation));

		SkyLight = NewObject<USkyLightComponent>(GetTransientPackage());
		SkyLight->Intensity = 0.5f;
		SkyLight->LightColor = FColor::White;
		AddComponent(SkyLight, FTransform(CVS.LightRotation));

		LineBatcher = NewObject<ULineBatchComponent>(GetTransientPackage());
		LineBatcher->bCalculateAccurateBounds = false;
		AddComponent(LineBatcher, FTransform::Identity);

		// 
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.bNoFail = true;
		SpawnInfo.ObjectFlags = RF_Transient;
		PreviewActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnInfo);

		PreviewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		PreviewActor->SetActorEnableCollision(false);
	}
}

void FDRPreviewScene::AddComponent(UActorComponent* Component, const FTransform& LocalToWorld)
{
	Components.AddUnique(Component);

	USceneComponent* SceneComp = Cast<USceneComponent>(Component);
	if (SceneComp && SceneComp->GetAttachParent() == NULL)
	{
		SceneComp->SetRelativeTransform(LocalToWorld);
	}

	Component->RegisterComponentWithWorld(GetWorld());

	if (bForceAllUsedMipsResident)
	{
		// Add a mip streaming override to the new mesh
		UMeshComponent* pMesh = Cast<UMeshComponent>(Component);
		if (pMesh != NULL)
		{
			pMesh->SetTextureForceResidentFlag(true);
		}
	}

	GetScene()->UpdateSpeedTreeWind(0.0);
}

void FDRPreviewScene::RemoveComponent(UActorComponent* Component)
{
	Component->UnregisterComponent();
	Components.Remove(Component);

	if (bForceAllUsedMipsResident)
	{
		// Remove the mip streaming override on the old mesh
		UMeshComponent* pMesh = Cast<UMeshComponent>(Component);
		if (pMesh != NULL)
		{
			pMesh->SetTextureForceResidentFlag(false);
		}
	}
}

void FDRPreviewScene::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(Components);

	if (Floor)
	{
		Collector.AddReferencedObject(Floor);
	}
	if (SkySphere)
	{
		Collector.AddReferencedObject(SkySphere);
	}
	if (DirectionalLight)
	{
		Collector.AddReferencedObject(DirectionalLight);
	}

	if (PreviewWorld)
	{
		Collector.AddReferencedObject(PreviewWorld);
	}
}

void FDRPreviewScene::ClearLineBatcher()
{
	if (LineBatcher != NULL)
	{
		LineBatcher->Flush();
	}
}

/** Accessor for finding the current direction of the preview scene's DirectionalLight. */
FRotator FDRPreviewScene::GetLightDirection()
{
	return DirectionalLight ? DirectionalLight->GetComponentToWorld().GetUnitAxis(EAxis::X).Rotation() : FRotator();
}

float FDRPreviewScene::GetLightBrightness()
{
	return DirectionalLight ? DirectionalLight->Intensity : 0;
}

FLinearColor FDRPreviewScene::GetLightColor()
{
	return DirectionalLight ? DirectionalLight->GetLightColor() : FLinearColor::White;
}

float FDRPreviewScene::GetSkyBrightness()
{
	return SkyLight ? SkyLight->Intensity : 0;
}


/** Function for modifying the current direction of the preview scene's DirectionalLight. */
void FDRPreviewScene::SetLightDirection(const FRotator& InLightDir)
{
	if (DirectionalLight)
	{
		DirectionalLight->SetAbsolute(true, true, true);
		DirectionalLight->SetRelativeRotation(InLightDir);
	}
}

void FDRPreviewScene::SetLightBrightness(float LightBrightness)
{
	if (DirectionalLight)
	{
		DirectionalLight->SetIntensity(LightBrightness);
	}
}

void FDRPreviewScene::SetLightColor(const FColor& LightColor)
{
	if (DirectionalLight)
	{
		DirectionalLight->SetLightColor(LightColor);
	}
}

void FDRPreviewScene::SetSkyBrightness(float SkyBrightness)
{
	if (SkyLight)
	{
		SkyLight->SetIntensity(SkyBrightness);
	}
}

void FDRPreviewScene::LoadSettings(const TCHAR* Section)
{
	FRotator LightDir;
	if (GConfig->GetRotator(Section, TEXT("LightDir"), LightDir, GEditorPerProjectIni))
	{
		SetLightDirection(LightDir);
	}
}

void FDRPreviewScene::SetSkySphereMat(UMaterialInterface* mat)
{
	if (SkySphere)
	{
		if (AStaticMeshActor * ac = Cast<AStaticMeshActor>(SkySphere))
		{
			if (UStaticMeshComponent* sc = ac->GetStaticMeshComponent())
				sc->SetMaterial(0, mat);
		}
	}
}

void FDRPreviewScene::SetFloorVisible(bool bVisible)
{
	if (Floor)
	{
		Floor->SetActorHiddenInGame(!bVisible);
	}
}

bool FDRPreviewScene::IsFloorVisible()
{
	return Floor && !Floor->bHidden;
}

void FDRPreviewScene::SaveSettings(const TCHAR* Section)
{
	GConfig->SetRotator(Section, TEXT("LightDir"), GetLightDirection(), GEditorPerProjectIni);
}

float FDRPreviewScene::GetBoundsZOffset(const FBoxSphereBounds& Bounds) const
{
	// Return half the height of the bounds plus one to avoid ZFighting with the floor plane
	return Bounds.BoxExtent.Z + 1;
}

bool FDRPreviewScene::IsValidComponentForVisualization(USceneComponent* Component) const
{
	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component);
	if (PrimComp && PrimComp->IsVisible() && !PrimComp->bHiddenInGame)
	{
		UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Component);
		if (StaticMeshComp && StaticMeshComp->GetStaticMesh())
		{
			return true;
		}

		USkeletalMeshComponent* SkelMeshComp = Cast<USkeletalMeshComponent>(Component);
		if (SkelMeshComp && SkelMeshComp->SkeletalMesh)
		{
			return true;
		}
	}
	return false;
}

FBoxSphereBounds FDRPreviewScene::GetPreviewActorBounds() const
{
	FBoxSphereBounds Bounds(ForceInitToZero);
	if (PreviewActor && PreviewActor->GetRootComponent())
	{
		TArray<USceneComponent*> PreviewComponents;
		PreviewActor->GetRootComponent()->GetChildrenComponents(true, PreviewComponents);
		PreviewComponents.Add(PreviewActor->GetRootComponent());

		for (USceneComponent* PreviewComponent : PreviewComponents)
		{
			if (IsValidComponentForVisualization(PreviewComponent))
			{
				Bounds = Bounds + PreviewComponent->Bounds;
			}
		}
	}
	return Bounds;
}

void FDRPreviewScene::GetViewMatrixParameters(const float InFOVDegrees, FVector& OutOrigin, float& OutOrbitPitch, float& OutOrbitYaw, float& OutOrbitZoom) const
{
	const float HalfFOVRadians = FMath::DegreesToRadians<float>(InFOVDegrees) * 0.5f;
	// Add extra size to view slightly outside of the sphere to compensate for perspective
	const FBoxSphereBounds Bounds = GetPreviewActorBounds();

	const float HalfMeshSize = Bounds.SphereRadius * 1.15;
	const float BoundsZOffset = GetBoundsZOffset(Bounds);
	const float TargetDistance = HalfMeshSize / FMath::Tan(HalfFOVRadians);

	OutOrigin = FVector::ZeroVector;// FVector(0, 0, -BoundsZOffset);
	OutOrbitPitch = 0;
	OutOrbitYaw = 0;
	OutOrbitZoom = TargetDistance;
}

void FDRPreviewScene::GetView(FSceneViewFamily* ViewFamily, int32 X, int32 Y, uint32 SizeX, uint32 SizeY) const
{
	check(ViewFamily);

	FIntRect ViewRect(
		FMath::Max<int32>(X, 0),
		FMath::Max<int32>(Y, 0),
		FMath::Max<int32>(X + SizeX, 0),
		FMath::Max<int32>(Y + SizeY, 0));

	if (ViewRect.Width() > 0 && ViewRect.Height() > 0)
	{
		const float FOVDegrees = 30.f;
		const float HalfFOVRadians = FMath::DegreesToRadians<float>(FOVDegrees) * 0.5f;
		static_assert((int32)ERHIZBuffer::IsInverted != 0, "Check NearPlane and Projection Matrix");
		const float NearPlane = 1.0f;
		FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
			HalfFOVRadians,
			1.0f,
			1.0f,
			NearPlane
		);

		FVector Origin(0);
		float OrbitPitch = 0;
		float OrbitYaw = 0;
		float OrbitZoom = 0;
		GetViewMatrixParameters(FOVDegrees, Origin, OrbitPitch, OrbitYaw, OrbitZoom);

		// Ensure a minimum camera distance to prevent problems with really small objects
		const float MinCameraDistance = 48;
		OrbitZoom = FMath::Max<float>(MinCameraDistance, OrbitZoom);

		const FRotator RotationOffsetToViewCenter(0.f, 90.f, 0.f);
		FMatrix ViewRotationMatrix = FRotationMatrix(FRotator(0, OrbitYaw, 0)) *
			FRotationMatrix(FRotator(0, 0, OrbitPitch)) *
			FTranslationMatrix(FVector(0, OrbitZoom, 0)) *
			FInverseRotationMatrix(RotationOffsetToViewCenter);

		ViewRotationMatrix = ViewRotationMatrix * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));

		Origin -= ViewRotationMatrix.InverseTransformPosition(FVector::ZeroVector);
		ViewRotationMatrix = ViewRotationMatrix.RemoveTranslation();

		FSceneViewInitOptions ViewInitOptions;
		ViewInitOptions.ViewFamily = ViewFamily;
		ViewInitOptions.SetViewRectangle(ViewRect);
		ViewInitOptions.ViewOrigin = -Origin;
		ViewInitOptions.ViewRotationMatrix = ViewRotationMatrix;
		ViewInitOptions.ProjectionMatrix = ProjectionMatrix;
		ViewInitOptions.BackgroundColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

		FSceneView* NewView = new FSceneView(ViewInitOptions);

		ViewFamily->Views.Add(NewView);
	}
}

void FDRPreviewScene::Draw(uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas)
{
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(RenderTarget, GetScene(), FEngineShowFlags(ESFIM_Game))
		.SetWorldTimes(FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime));

	ViewFamily.EngineShowFlags.MotionBlur = 0;
	ViewFamily.EngineShowFlags.AntiAliasing = 1;

	GetView(&ViewFamily, 0, 0, Width, Height);

	if (ViewFamily.Views.Num() > 0)
	{
		GetRendererModule().BeginRenderingViewFamily(Canvas, &ViewFamily);
	}
}

void FDRPreviewScene::SetActorVisible(const TSubclassOf<AActor> &ExceptActorClassType, bool bVisible)
{
	if (PreviewWorld)
	{
		for (FActorIterator It(PreviewWorld); It; ++It)
		{
			AActor *pActor = *It;
			if (!pActor)
			{
				continue;
			}

			if (ExceptActorClassType && pActor->IsA(ExceptActorClassType))
			{
				continue;
			}

			if (pActor->IsA(ALight::StaticClass()))
			{
				continue;
			}

			if (pActor)
			{
				pActor->SetActorHiddenInGame(!bVisible);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
FDRMaterialScene::FDRMaterialScene()
{
}

void FDRMaterialScene::SetMaterialInterface(class UMaterialInterface *InMaterial, UStaticMesh *PreviewMesh)
{
	if (InMaterial && PreviewActor && PreviewMesh)
	{
		UStaticMeshComponent *MeshComponent = PreviewActor->GetStaticMeshComponent();
		if (MeshComponent)
		{
			// Transform the preview mesh as necessary
			UMaterial* BaseMaterial = InMaterial->GetBaseMaterial();

			FTransform Transform = FTransform::Identity;
			Transform.SetScale3D(FVector(3.0f));
			Transform.SetRotation(FQuat(FRotator(180, 0, 0)));

			if (PreviewActor)
			{
				PreviewActor->GetStaticMeshComponent()->SetStaticMesh(PreviewMesh);
				PreviewActor->GetStaticMeshComponent()->SetRelativeTransform(Transform);
				PreviewActor->GetStaticMeshComponent()->UpdateBounds();

				PreviewActor->GetStaticMeshComponent()->SetMaterial(0, InMaterial);
				PreviewActor->GetStaticMeshComponent()->RecreateRenderState_Concurrent();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
FDRModelFileScene::FDRModelFileScene()
{
	ModelFileComp = NewObject<UModelFileComponent>(PreviewActor);
	AddComponent(ModelFileComp, FTransform::Identity);
}

FBoxSphereBounds FDRModelFileScene::GetPreviewActorBounds() const
{
	FBoxSphereBounds Bounds = FDRPreviewScene::GetPreviewActorBounds();
	Bounds = Bounds + ModelFileComp->Bounds;
	return Bounds;
}

void FDRModelFileScene::SetModel(class UModelFile *Model)
{
	if (Model)
	{
		FBoxSphereBounds Bounds(Model->LocalBounds);
		const float BoundsZOffset = GetBoundsZOffset(Bounds);

		FTransform Transform;
		Transform.SetLocation(-Bounds.Origin + FVector(0, 0, BoundsZOffset));
		ModelFileComp->SetRelativeTransform(Transform);

		ModelFileComp->UpdateModel(Model);
	}
}




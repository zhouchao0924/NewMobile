
#include "ajdr.h"
#include "DRLevelScene.h"

FDRLevelScene::FDRLevelScene(const FString &InLevelName)
	:LevelName(InLevelName)
{
	Floor = NULL;
	DirectionalLight = NULL;
	SkySphere = NULL;
}

FDRLevelScene::~FDRLevelScene()
{
	PreviewWorld->CleanupWorld(true, false);
	GEngine->DestroyWorldContext(GetWorld());
}

void FDRLevelScene::ConstructWorld()
{
	UPackage *WorldPackage = LoadPackage(NULL, *LevelName, LOAD_None);

	if (WorldPackage)
	{
		PreviewWorld = NewObject<UWorld>();
		PreviewWorld->WorldType = EWorldType::GamePreview;

		PreviewWorld->SetFlags(RF_Transactional);

		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(PreviewWorld->WorldType);
		WorldContext.SetCurrentWorld(PreviewWorld);

		PreviewWorld->InitializeNewWorld(UWorld::InitializationValues()
			.AllowAudioPlayback(false)
			.CreatePhysicsScene(true)
			.RequiresHitProxies(true)
			.CreateNavigation(false)
			.CreateAISystem(false)
			.ShouldSimulatePhysics(true)
			.SetTransactional(true));

		PreviewWorld->InitializeActorsForPlay(FURL());

		UWorld *FoundWorld = UWorld::FindWorldInPackage(WorldPackage);
		if (FoundWorld)
		{
			FoundWorld->PersistentLevel->bIsVisible = 1;

			for (FActorIterator It(FoundWorld); It; ++It)
			{
				AActor *SrcActor = *It;

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Template = SrcActor;

				AActor *NewActor = PreviewWorld->SpawnActor<AActor>(SrcActor->GetClass(), SrcActor->GetActorTransform(), SpawnParameters);

				if (ADirectionalLight *DirectionalLightActor = Cast<ADirectionalLight>(NewActor))
				{
					DirectionalLight = Cast<UDirectionalLightComponent>(DirectionalLightActor->GetLightComponent());
				}

				if (ASkyLight *SkyLightActor = Cast<ASkyLight>(NewActor))
				{
					SkyLight = Cast<USkyLightComponent>(SkyLightActor->GetLightComponent());
					SkyLight->Activate(true);
				}

				if (APlayerStart *PlayStartActor = Cast<APlayerStart>(NewActor))
				{
					FActorSpawnParameters SpawnInfo;
					SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					SpawnInfo.bNoFail = true;
					SpawnInfo.ObjectFlags = RF_Transient;

					PreviewActor = PreviewWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), PlayStartActor->GetTransform(), SpawnInfo);
					PreviewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
					PreviewActor->SetActorEnableCollision(false);
				}

				if (APostProcessVolume *PostVolumeActor = Cast<APostProcessVolume>(NewActor))
				{
					PostVolumeActor->SetActorLocation(SrcActor->GetActorLocation());
					PostVolumeActor->SetActorScale3D(SrcActor->GetActorScale3D());
					ProcessVolume = PostVolumeActor;
				}

				if (SrcActor->GetName() == TEXT("Floor"))
				{
					Floor = NewActor;
				}
				if (SrcActor->GetName().Contains("SM_SkySphere"))
				{
					SkySphere = NewActor;
				}

				// Update sky light first because it's considered direct lighting, sky diffuse will be visible in reflection capture indirect specular
				if (PreviewWorld)
				{
					PreviewWorld->UpdateAllSkyCaptures();
				}

				USkyLightComponent::UpdateSkyCaptureContents(PreviewWorld);
				UReflectionCaptureComponent::UpdateReflectionCaptureContents(PreviewWorld);
			}
		}
	}
}

void FDRLevelScene::OverridePostProcessSettings(FSceneView *View)
{
	if (ProcessVolume.IsValid())
	{
		FPostProcessVolumeProperties Properties = ProcessVolume->GetProperties();
		if (Properties.bIsEnabled && Properties.Settings)
		{
			View->OverridePostProcessSettings(*Properties.Settings, 1.0f);
		}
	}
}


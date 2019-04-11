// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "DRGameMode.h"
#include "DRMainFrame.h"
#include "EditModeManager.h"
#include "DREditorViewportWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DRDrawer.h"
#include "BuildingSDK.h"
#include "DRPreviewScene.h"
#include "VaRestRequestJSON.h"
#include "VaRestJsonObject.h"
#include "ResourceMgr.h"

ADRGameMode::ADRGameMode(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, MainFrame(nullptr)
	, ModeManager(nullptr)
	, DRMaterialScene(nullptr)
	, BuildingSDK(nullptr)
	, PreviewMaterialMesh(nullptr)
	, State(EState::EOffline)
{
	Business = ObjectInitializer.CreateDefaultSubobject<UBusinessComponent>(this, TEXT("Business"));
	ContentCategoryTree = ObjectInitializer.CreateDefaultSubobject<UContentCategoryTreeComponent>(this, TEXT("ContentCategoryTree"));
}

ADRGameMode *ADRGameMode::GetDRGameMode(UObject *WorldContextObject)
{
	ADRGameMode *MyGame = nullptr;

	if (WorldContextObject)
	{
		UWorld *MyWorld = WorldContextObject->GetWorld();
		MyGame = MyWorld ? Cast<ADRGameMode>(MyWorld->GetAuthGameMode()) : nullptr;
	}

	return MyGame;
}

UBusinessComponent *ADRGameMode::GetBusiness(UObject *WorldContextObject)
{
	ADRGameMode *MyGame = GetDRGameMode(WorldContextObject);
	if (MyGame)
	{
		return MyGame->Business;
	}
	return nullptr;
}

UContentCategoryTreeComponent *ADRGameMode::GetContent(UObject *WorldContextObject)
{
	ADRGameMode *MyGame = GetDRGameMode(WorldContextObject);
	if (MyGame)
	{
		return MyGame->ContentCategoryTree;
	}
	return nullptr;
}

void ADRGameMode::SetCurrentBuildingSystem(UBuildingSystem *InBuildingSystem)
{
	if (BuildingSystem)
	{
		BuildingSystem->ClearSuite();
		BuildingSystem->ConditionalBeginDestroy();
		BuildingSystem = nullptr;
	}
	
	BuildingSystem = InBuildingSystem;

	if (ModeManager)
	{
		ModeManager->SetBuildingSystem(BuildingSystem);
	}
}

UBuildingSystem *ADRGameMode::LoadNewSuite(const FString &InFilename)
{
	UBuildingSystem *NewBuildingSystem = UBuildingSystem::LoadNewSuite(this, InFilename);
	SetCurrentBuildingSystem(NewBuildingSystem);
	return NewBuildingSystem;
}

UBuildingSystem *ADRGameMode::CreateNewSuite(const FString &InFilename /*= TEXT("")*/)
{
	UBuildingSystem *NewBuildingSystem = UBuildingSystem::CreateNewSuite(this, InFilename);
	SetCurrentBuildingSystem(NewBuildingSystem);
	return NewBuildingSystem;
}

UBuildingSystem *ADRGameMode::GetBuildingSystem(UObject *WorldContextObject)
{
	ADRGameMode *MyGame = GetDRGameMode(WorldContextObject);
	if (MyGame)
	{
		return MyGame->BuildingSystem;
	}
	return nullptr;
}

UEditModeManager *ADRGameMode::GetModeManager(UObject *WorldContextObject)
{
	ADRGameMode *MyGame = GetDRGameMode(WorldContextObject);
	if (MyGame)
	{
		return MyGame->ModeManager;
	}
	return nullptr;
}

UDRMainFrame *ADRGameMode::GetMainFrame(UObject *WorldContextObject)
{
	ADRGameMode *MyGame = GetDRGameMode(WorldContextObject);
	if (MyGame)
	{
		return MyGame->MainFrame;
	}
	return nullptr;
}

UDREditorViewportWidget *ADRGameMode::GetViewportClientWidget(UObject *WorldContextObject)
{
	UDRMainFrame *MainFrm = GetMainFrame(WorldContextObject);
	if (MainFrm)
	{
		return MainFrm->GetViewportWidget();
	}
	return nullptr;
}

FDREditorViewportClient *ADRGameMode::GetViewportClient(UObject *WorldContextObject)
{
	FDREditorViewportClient *ViewportClient = nullptr;
	UDREditorViewportWidget *ViewportWidget = GetViewportClientWidget(WorldContextObject);
	if (ViewportWidget)
	{
		ViewportClient = ViewportWidget->GetViewportClient();
	}
	return ViewportClient;
}

void ADRGameMode::BeginPlay()
{
	if (Business)
	{
		InitBusiness();
	}

	Super::BeginPlay();

	UResourceMgr *ResMgr = UResourceMgr::Create(this);
	if (ResMgr)
	{
		ResMgr->AddToRoot();
	}

	if (MateralCollectionConfig)
	{
		UStandardMaterialCollection *DefMaterialCollection = MateralCollectionConfig->GetDefaultObject<UStandardMaterialCollection>();
		if (DefMaterialCollection)
		{
			UStandardMaterialCollection::SetMaterialCollection(DefMaterialCollection);
		}
	}
}

void ADRGameMode::InitBusiness()
{
	if (Business && !Business->OnBusinessHandler.IsBound())
	{
		Business->OnBusinessHandler.AddDynamic(this, &ADRGameMode::RequestHandler);
	}
}

void ADRGameMode::Startup()
{
	// main editor framework
	if (UMGMainFrame)
	{
		MainFrame = CreateWidget<UDRMainFrame>(GetWorld(), UMGMainFrame);
		MainFrame->AddToViewport();
	}

	if (MainFrame)
	{
		APlayerController *MyController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (MyController)
		{
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(MyController, MainFrame, EMouseLockMode::DoNotLock);
		}
	}

	//init editor controller 
	if (EditModeManagerType)
	{
		ModeManager = NewObject<UEditModeManager>(this, EditModeManagerType, TEXT("EditModeManager"));
		if (ModeManager)
		{
			ModeManager->BeginPlay();
			ModeManager->SetEditMode(EEditMode::Design3D);
		}
	}

	//create new suite
	UBuildingSystem *NewBuildingSystem = CreateNewSuite();
	if (NewBuildingSystem)
	{
		NewBuildingSystem->AddToWorld(GetWorld());
	}
}

void ADRGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UResourceMgr *ResMgr = UResourceMgr::GetResourceMgr();
	if (ResMgr)
	{
		ResMgr->RemoveFromRoot();
	}

	if (DRMaterialScene)
	{
		delete DRMaterialScene;
		DRMaterialScene = nullptr;
	}

	if (Business)
	{
		Business->OnBusinessHandler.RemoveDynamic(this, &ADRGameMode::RequestHandler);
	}

	UResourceMgr::Destroy();

	UStandardMaterialCollection::SetMaterialCollection(nullptr);

	Super::EndPlay(EndPlayReason);
}

FDRMaterialScene *ADRGameMode::GetMaterialPreviewScene()
{
	if (!DRMaterialScene)
	{
		DRMaterialScene = new FDRMaterialScene();
		DRMaterialScene->ConstructWorld();
	}
	return DRMaterialScene;
}

void ADRGameMode::SetState(EState InState)
{
	State = InState;
}

void ADRGameMode::RequestHandler(UVaRestRequestJSON *JSONRequest)
{
	if (JSONRequest)
	{
		UVaRestJsonObject *Json = JSONRequest->GetResponseObject();
		ERequestID RequestID = (ERequestID)JSONRequest->RequestID;
		switch (RequestID)
		{
		case ERequestID::ELoginID:
		{
			LoginHandler(Json);
			break;
		}
		case ERequestID::EQuerySeries:
		{
			CategoryHandler(Json);
			break;
		}
		case ERequestID::EQueryGoodsByCond:
		{
			GoodsQueryByCondHandler(Json);
			break;
		}
		case ERequestID::EQueryGoodsPageInfo:
		{
			GoodsPageInfoQueryHandler(Json);
			break;
		}
		default: break;
		}
	}
}




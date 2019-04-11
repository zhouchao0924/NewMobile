// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ContentCategoryTreeComponent.h"
#include "Business.h"
#include "Building/BuildingConfig.h"
#include "StandardMaterialCollection.h"
#include "DREditorViewportClient.h"
#include "DRGameMode.generated.h"

class UResourceMgr;
class UDRMainFrame;
class UEditModeManager;
class UDREditorViewportWidget;
class UDRDrawBlueprintLibrary;
class FDRPreviewScene;
class IBuildingSDK;

UENUM(BlueprintType)
enum class EState : uint8
{
	EOffline,
	ELogin,
};

UCLASS(BlueprintType)
class ADRGameMode :public AGameMode
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(Blueprintpure, meta = (WorldContext = "WorldContextObject"))
	static UEditModeManager *GetModeManager(UObject *WorldContextObject);

	UFUNCTION(Blueprintpure, meta = (WorldContext = "WorldContextObject"))
	static UDRMainFrame *GetMainFrame(UObject *WorldContextObject);

	UFUNCTION(Blueprintpure, meta = (WorldContext = "WorldContextObject"))
	static UDREditorViewportWidget *GetViewportClientWidget(UObject *WorldContextObject);

	UFUNCTION(Blueprintpure, meta = (WorldContext = "WorldContextObject"))
	static UBuildingSystem *GetBuildingSystem(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static ADRGameMode *GetDRGameMode(UObject *WorldContextObject);

	UFUNCTION(Blueprintpure, meta = (WorldContext = "WorldContextObject"))
	static UBusinessComponent *GetBusiness(UObject *WorldContextObject);

	UFUNCTION(Blueprintpure, meta = (WorldContext = "WorldContextObject"))
	static UContentCategoryTreeComponent *GetContent(UObject *WorldContextObject);

	UFUNCTION(Blueprintpure)
	UStaticMesh *GetPreviewMaterialMesh() { return PreviewMaterialMesh; }

	UFUNCTION(BlueprintCallable)
	UBuildingSystem *LoadNewSuite(const FString &InFilename);

	UFUNCTION(BlueprintCallable)
	UBuildingSystem *CreateNewSuite(const FString &InFilename = TEXT(""));

	UFUNCTION(BlueprintCallable)
	void Startup();

	UFUNCTION()
	void RequestHandler(UVaRestRequestJSON *JSONRequest);
	void SetState(EState InState);
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	FDRMaterialScene *GetMaterialPreviewScene();
	static FDREditorViewportClient *GetViewportClient(UObject *WorldContextObject);
	FBuildingConfig *GetConfig() { return &BuildingConfig; }
protected:
	void InitBusiness();
	void SetCurrentBuildingSystem(UBuildingSystem *InBuildingSystem);
	void LoginHandler(UVaRestJsonObject *ResponseObj);
	void CategoryHandler(UVaRestJsonObject *ResponseObj);
	void GoodsQueryByCondHandler(UVaRestJsonObject *ResponseObj);
	void GoodsPageInfoQueryHandler(UVaRestJsonObject *ResponseObj);
protected:
	UPROPERTY(Transient)
	UEditModeManager	*ModeManager;

	UPROPERTY(Transient)
	UDRMainFrame		*MainFrame;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDRMainFrame> UMGMainFrame;	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEditModeManager> EditModeManagerType;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UStandardMaterialCollection> MateralCollectionConfig;

	UPROPERTY(EditAnywhere)
	FBuildingConfig		BuildingConfig;

	UPROPERTY(EditAnywhere)
	UStaticMesh			*PreviewMaterialMesh;

	UPROPERTY(Transient)
	UBuildingSystem		*BuildingSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBusinessComponent	*Business;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UContentCategoryTreeComponent *ContentCategoryTree;

	UPROPERTY(Transient, BlueprintReadOnly)
	EState				State;

	FDRMaterialScene	*DRMaterialScene;
	IBuildingSDK		*BuildingSDK;
};





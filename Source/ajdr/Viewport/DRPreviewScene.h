// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PreviewScene.h"

class FDRPreviewScene : public FGCObject
{
public:
	struct ConstructionValues
	{
		ConstructionValues()
			: LightRotation(-40.f, -67.5f, 0.f)
			, SkyBrightness(0.f)
			, LightBrightness(PI)
			, bAllowAudioPlayback(false)
			, bForceMipsResident(true)
			, bCreatePhysicsScene(true)
			, bShouldSimulatePhysics(false)
			, bTransactional(true)
			, bEditor(true)
		{
		}

		FRotator LightRotation;
		float SkyBrightness;
		float LightBrightness;
		uint32 bAllowAudioPlayback : 1;
		uint32 bForceMipsResident : 1;
		uint32 bCreatePhysicsScene : 1;
		uint32 bShouldSimulatePhysics : 1;
		uint32 bTransactional : 1;
		uint32 bEditor : 1;

		ConstructionValues& SetLightRotation(const FRotator& Rotation) { LightRotation = Rotation; return *this; }
		ConstructionValues& SetSkyBrightness(const float Brightness) { SkyBrightness = Brightness; return *this; }
		ConstructionValues& SetLightBrightness(const float Brightness) { LightBrightness = Brightness; return *this; }
		ConstructionValues& AllowAudioPlayback(const bool bAllow) { bAllowAudioPlayback = bAllow; return *this; }
		ConstructionValues& SetForceMipsResident(const bool bForce) { bForceMipsResident = bForce; return *this; }
		ConstructionValues& SetCreatePhysicsScene(const bool bCreate) { bCreatePhysicsScene = bCreate; return *this; }
		ConstructionValues& ShouldSimulatePhysics(const bool bInShouldSimulatePhysics) { bShouldSimulatePhysics = bInShouldSimulatePhysics; return *this; }
		ConstructionValues& SetTransactional(const bool bInTransactional) { bTransactional = bInTransactional; return *this; }
		ConstructionValues& SetEditor(const bool bInEditor) { bEditor = bInEditor; return *this; }
	};

	// for physical correct light computations we multiply diffuse and specular lights by PI (see LABEL_RealEnergy)
	FDRPreviewScene(const ConstructionValues &CVS = ConstructionValues());
	virtual ~FDRPreviewScene();

	virtual void ConstructWorld();
	virtual void AddComponent(class UActorComponent* Component, const FTransform& LocalToWorld);
	virtual void RemoveComponent(class UActorComponent* Component);

	// Serializer.
	void AddReferencedObjects(FReferenceCollector& Collector) override;

	// Accessors.
	UWorld* GetWorld() const { return PreviewWorld; }
	FSceneInterface* GetScene() const { return PreviewWorld->Scene; }

	/** Access to line drawing */
	class ULineBatchComponent* GetLineBatcher() const { return LineBatcher; }
	/** Clean out the line batcher each frame */
	void ClearLineBatcher();

	virtual FRotator GetLightDirection();
	virtual float GetLightBrightness();
	virtual FLinearColor GetLightColor();
	virtual float GetSkyBrightness();

	virtual void SetLightDirection(const FRotator& InLightDir);
	virtual void SetLightBrightness(float LightBrightness);
	virtual void SetLightColor(const FColor& LightColor);
	virtual void SetSkyBrightness(float SkyBrightness);

	/** Load/Save settings to the config, specifying the key */
	virtual void LoadSettings(const TCHAR* Section);
	virtual void SaveSettings(const TCHAR* Section);
	virtual void OverridePostProcessSettings(FSceneView *View) {}

	void SetSkySphereMat(UMaterialInterface* mat);
	void SetFloorVisible(bool bVisible);
	bool IsFloorVisible();
	void Clean();

	FTransform GetPreviewTransform() { return PreviewActor ? PreviewActor->GetTransform() : FTransform::Identity; }
	AActor *GetPreviewActor() { return PreviewActor; }

	void  Draw(uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas);
	void  SetActorVisible(const TSubclassOf<AActor> &ExceptActorClassType, bool bVisible);
	class UDirectionalLightComponent* DirectionalLight;
	class USkyLightComponent *SkyLight;
private:
	TArray<class UActorComponent*> Components;
protected:
	bool IsValidComponentForVisualization(USceneComponent* Component) const;
	void GetView(FSceneViewFamily* ViewFamily, int32 X, int32 Y, uint32 SizeX, uint32 SizeY) const;
	void GetViewMatrixParameters(const float InFOVDegrees, FVector& OutOrigin, float& OutOrbitPitch, float& OutOrbitYaw, float& OutOrbitZoom) const;
	float GetBoundsZOffset(const FBoxSphereBounds& Bounds) const;
	virtual FBoxSphereBounds GetPreviewActorBounds() const;
protected:
	FDRPreviewScene::ConstructionValues CVS;
	class AStaticMeshActor				*PreviewActor;
	class UWorld						*PreviewWorld;
	class ULineBatchComponent			*LineBatcher;
	AActor								*Floor;
	AActor								*SkySphere;
	bool								bInit;
	bool								bForceAllUsedMipsResident;
};

class FDRMaterialScene :public FDRPreviewScene
{
public:
	FDRMaterialScene();
	void  SetMaterialInterface(class UMaterialInterface *Material, UStaticMesh *PreviewMesh);
};

class FDRModelFileScene :public FDRPreviewScene
{
public:
	FDRModelFileScene();
	void SetModel(class UModelFile *Model);
	FBoxSphereBounds GetPreviewActorBounds() const override;
protected:
	class UModelFileComponent *ModelFileComp;
};



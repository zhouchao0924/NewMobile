// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingData.h"
#include "ProceduralMeshComponent.h"
#include "WallPlane.h"
#include "BuildingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNotifyUpdateSurface, int32, SectionIndex, int32, SurfaceID, int32, ComponetType);
const FString DEF_TOP_WALL_MATERIAL(TEXT("Material'/Game/_Basic/Materials/Root/M_WallArea_Inst.M_WallArea_Inst'"));
const FString DEFAULT_WALL_MATERIAL(TEXT("Material'/Game/Common/DefaultWallMaterial.DefaultWallMaterial'"));
const FString DEFAULT_AREA_MATERIAL(TEXT("Material'/Game/_Basic/Materials/Root/M_WallArea_Inst.M_WallArea_Inst'"));

UCLASS(BlueprintType)
class UBuildingComponent :public UProceduralMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetData(UBuildingData *Data);

	UFUNCTION(BlueprintCallable)
	void UpdateSurface(int32 SectionIndex, int32 SurfaceID);
	
	//获取点击的SurfaceID
	UFUNCTION(BlueprintCallable)
	int32 HitTestSurface(const FVector &RayStart, const FVector &RayDir) const;
	//获取点击的SubModelIndex
	UFUNCTION(BlueprintCallable)
	int32 HitTestSubModelIndex(const FVector &RayStart, const FVector &RayDir);

	///////////////////////material///////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable)
	void SetSaveUVData(const FVector2D& UV, const FVector2D& UVOffset, const float& Angle);
	UFUNCTION(BlueprintCallable)
	void SetMatFloatParam(UPARAM(ref) FString& ParamName, const float& Value);
	/*UFUNCTION(BlueprintCallable)
	void SetMat2DParam(UPARAM(ref) FString& ParamName, const FVector2D& Value);
	UFUNCTION(BlueprintCallable)
	void SetMat3DParam(UPARAM(ref) FString& ParamName, const FVector& Value);
	UFUNCTION(BlueprintCallable)*/
	void SetMat4DParam(UPARAM(ref) FString& ParamName, const FVector4& Value);
	void SetMatDefaultParams(const int32& SurfaceID);
	void SetMatParam(const int32& SurfaceID, const FString& ParamName, IValue* Value);
	UFUNCTION(BlueprintCallable)
	void SetMatParams(const int32& SurfaceID, const FVector2D& UVScale, const FVector2D& UVOffset, const float& Angle, const FVector2D& DefaultUV);
	UFUNCTION(BlueprintCallable)
	void GetMatParams(const int32& SurfaceID, FVector2D& UVScale, FVector2D& UVOffset, float& Angle, FVector2D& DefaultUV);
	//ÉèÖÃÐÂ²ÄÖÊ
	UFUNCTION(BlueprintCallable)
	void SetNewMaterial(const FVector &RayStart, const FVector &RayDir, const int32& MaterialType, const int32& ModelID, UPARAM(ref)FString& ResID);

	UMaterialInterface * GetMaterialInterfaceBySurfaceID(const int32& SurfaceID);
	void AutoSetMatParams(const int32& SectionIndex, const int32& SurfaceID, UMaterialInterface *UE4Mat);
	float GetMatDefaultFloatParam(const UMaterialInterface *UE4Mat, const FString& ParamName);
	///////////////////////material///////////////////////////////////////////////////

public:
	//0±íÊ¾ÊÇµØ°å£¬ÊÇPGround£¬1±íÊ¾ÊÇÌì»¨°å£¬ÊÇroof, 2±íÊ¾ÊÇLitMesh
	UPROPERTY(Transient, BlueprintReadWrite)
		int32 ComponetType;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "DR|Resource")
		FOnNotifyUpdateSurface					OnNotifyUpdateSurface;
	UPROPERTY(Transient, BlueprintReadOnly)
	FVector2D UVDefault;
	UPROPERTY(Transient, BlueprintReadOnly)
	UBuildingData *Data;
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 HitSurfaceID;
	AWallPlaneBase *WallPlane;
};



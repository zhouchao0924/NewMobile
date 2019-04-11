// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingData.h"
#include "ProceduralMeshComponent.h"
#include "BuildingComponent.generated.h"

UCLASS(BlueprintType)
class UBuildingComponent :public UProceduralMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetData(UBuildingData *Data);

	UFUNCTION(BlueprintCallable)
		void UpdateSurface(int32 SectionIndex, int32 SurfaceID);

	UFUNCTION(BlueprintCallable)
		int32 HitTestSurface(const FVector &RayStart, const FVector &RayDir) const;

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	UBuildingData *Data;
};



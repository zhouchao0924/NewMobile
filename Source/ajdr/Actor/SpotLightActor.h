// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DRActor.h"
#include "SpotLightActor.generated.h"

UCLASS(BlueprintType)
class ASpotLightActor :public ADRActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Update(UBuildingData *Data,float size);
protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	USpotLightComponent *SpotLightComponent;
};



// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DRActor.h"
#include "ModelFileComponent.h"
#include "DWActor.generated.h"

UCLASS(BlueprintType)
class ADWActor :public ADRActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Update(FString ResID);
	static void UpdateCompoennt(UModelFileComponent *InModelFileComponent,FString ResID);
	EVisibleChannel GetChannel() override { return EVisibleChannel::EModelChannel; }
protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	UModelFileComponent *DWModelComponentFile;
};



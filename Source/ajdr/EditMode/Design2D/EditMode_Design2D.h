// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditMode.h"
#include "Building/BuildingConfig.h"
#include "EditMode_Design2D.generated.h"

UCLASS(Blueprintable)
class UEditMode_Design2D : public UEditMode
{
	GENERATED_UCLASS_BODY()
public:
	void OnEnter() override;
	void Draw2D(FDrawer *Drawer) override;
	EEditMode GetMode() override { return EEditMode::Design2D; }
	void OnCheckObjectVisible(FObjectInfo &ObjInfo) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Mode")
	FBuildingConfig Config;
};



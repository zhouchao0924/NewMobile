// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditMode.h"
#include "EditMode_Design3D.generated.h"

UCLASS(Blueprintable)
class UEditMode_Design3D : public UEditMode
{
	GENERATED_UCLASS_BODY()
public:
	void OnEnter() override;
	EEditMode GetMode() override { return EEditMode::Design3D; }
	void OnCheckObjectVisible(FObjectInfo &ObjInfo) override;
};



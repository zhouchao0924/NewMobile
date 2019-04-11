// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CameraMod.h"

class FCameraMod_2D :public FCameraMod
{
public:
	void Reset() override;
	bool InputKey(FKey Key, EInputEvent Event) override;
	bool InputAxis(FKey Key, float Delta, float DeltaTime) override;
	ECameraMod GetMod() override { return ECameraMod::Camera2D; }
protected:
	void Move2D(const FVector2D &MoveDelta);
	void Scale2D(float Delta);
protected:
	FVector2D OriginalMousePosition;
	FVector	  OriginalViewLocation;
};



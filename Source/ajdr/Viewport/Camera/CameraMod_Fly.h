// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CameraMod.h"


class FCameraMod_Fly :public FCameraMod
{
public:
	void Reset() override;
	bool InputKey(FKey Key, EInputEvent Event) override;
	bool InputAxis(FKey Key, float Delta, float DeltaTime) override;
	ECameraMod GetMod() override { return ECameraMod::Fly; }
};



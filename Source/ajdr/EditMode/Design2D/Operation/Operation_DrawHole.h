// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operation.h"
#include "Operation_DrawHole.generated.h"

UCLASS(BlueprintType)
class UOperation_DrawDoor : public UOperation
{
	GENERATED_UCLASS_BODY()
public:
	EOperationType GetType() override { return EOperationType::EOperation_DrawDoor; }
	void InputKey(FViewport *Viewport, FKey Key, EInputEvent Event) override;
	void InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime) override;
};

UCLASS(BlueprintType)
class UOperation_DrawWindow : public UOperation
{
	GENERATED_UCLASS_BODY()
public:
	EOperationType GetType() override { return EOperationType::EOperation_DrawWindow; }
	void InputKey(FViewport *Viewport, FKey Key, EInputEvent Event) override;
	void InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime) override;
};




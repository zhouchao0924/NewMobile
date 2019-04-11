// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operation.h"
#include "Operation_Select.generated.h"

UCLASS(BlueprintType)
class UOperation_Select : public UOperation
{
	GENERATED_UCLASS_BODY()
public:
	EOperationType GetType() override { return EOperationType::EOperation_Select; }
	void InputKey(FViewport *Viewport, FKey Key, EInputEvent Event) override;
	void InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime) override;
};



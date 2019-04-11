// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DRActor.h"
#include "SceneManager.generated.h"

UCLASS(BlueprintType)
class ASceneManager : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void AddActor(ADRActor *InDRActor);
public:
	TArray<ADRActor *> Actors;
};



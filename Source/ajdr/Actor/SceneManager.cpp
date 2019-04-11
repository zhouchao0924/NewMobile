// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "SceneManager.h"

ASceneManager::ASceneManager(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void ASceneManager::AddActor(ADRActor *InDRActor)
{
	if (InDRActor)
	{
		Actors.Add(InDRActor);
	}
}


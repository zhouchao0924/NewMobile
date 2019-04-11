// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DRPreviewScene.h"

class FDRLevelScene : public FDRPreviewScene
{
public:
	FDRLevelScene(const FString &LevelName);
	virtual ~FDRLevelScene();
	void ConstructWorld() override;
	void OverridePostProcessSettings(FSceneView *View) override;
protected:
	FString								LevelName;
	TWeakObjectPtr<APostProcessVolume>	ProcessVolume;
};




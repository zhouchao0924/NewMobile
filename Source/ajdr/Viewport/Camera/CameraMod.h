// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ECameraMod : uint8
{
	Watch,
	Fly,
	Walk,
	Camera2D,
	CameraNone
};

class FDREditorViewportClient;

struct FCameraState
{
	FCameraState() 
		:bSaved(false)
		,bProjView(true)
	{
	}
	bool		bSaved;
	bool		bProjView;
	FVector		ViewLoc;
	float		OrthoScale;
	FRotator	ViewRotation;
};

class SDRViewport;

class FCameraMod
{
public:
	FCameraMod();
	virtual ~FCameraMod() {}
	virtual void OnEnter();
	virtual void OnLeave();
	virtual void Reset() {}
	virtual bool InputKey(FKey Key, EInputEvent Event) { return false;  }
	virtual bool InputAxis(FKey Key, float Delta, float DeltaTime) { return false; }
	virtual ECameraMod GetMod() = 0;
	void SetViewportClient(FDREditorViewportClient *InViewportClient);
protected:
	FDREditorViewportClient		*ViewportClient;
	FCameraState				SavedState;
};


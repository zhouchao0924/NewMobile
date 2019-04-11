// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CameraMod.h"

class SDRViewport;
class FCameraManager
{
public:
	struct FConfig
	{
		FConfig()
			: ZoomSpeed(1.0f)
			, ViewYawSpeed(-0.1f)
			, ViewPitchSpeed(-0.1f)
			, MoveSpeed(30.0f)
		{
		}
		float ZoomSpeed;
		float MoveSpeed;
		float ViewYawSpeed;
		float ViewPitchSpeed;
	};
	FCameraManager();
	void		SetCameraMod(ECameraMod Mod);
	ECameraMod  GetCurrentModType();
	FCameraMod *GetCurrentMod();
	FVector		GetViewForward();
	FVector		GetViewUp();
	FVector		GetViewRight();
	void	 GetViewPoint(FVector &out_CamLoc, FRotator &out_CamRotation);
	void	 SetViewLocation(const FVector& NewLocation) { ViewLocation = NewLocation; }
	void	 SetViewRotation(const FRotator& NewRotator) { ViewRotation = NewRotator; }
	FVector  GetViewLocation() { return ViewLocation;  }
	FRotator GetViewRotation() { return ViewRotation; }
	FRotator UpdateViewRotation(const FVector &DeltaMove);
	FVector  UpdateViewLcation(const FVector &DeltaMove);
	float	 GetFOVAngle() { return FOVAngle;  }
	void	 SetFOVAngle(float InFOV) { FOVAngle = InFOV; }
	void	 SetViewportClient(FDREditorViewportClient *InViewportClient);
	void	 SetViewType(bool bInProjectiveView) { bViewProject = bInProjectiveView; }
	bool	 IsProjView() { return bViewProject; }
	void	 GetMinimalViewInfo(FMinimalViewInfo &MinimalViewInfo);
	float	 GetOrthoScale() { return OrthoScale; }
	void	 SetOrthoScale(float InScale);
	friend class FDREditorViewportClient;
protected:
	TArray<FCameraMod *>			CameraMods;
	ECameraMod						CurrentMod;
 	FConfig							CameraConfig;
	FVector							ViewLocation;
	FRotator						ViewRotation;
	float							FOVAngle;
	float							OrthoScale;
	bool							bViewProject;
	float							NearClipPlane;
	float							FarClipPlane;
	FDREditorViewportClient			*ViewportClient;
};


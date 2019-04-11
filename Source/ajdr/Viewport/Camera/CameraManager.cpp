
#include "ajdr.h"
#include "CameraManager.h"
#include "CameraMod_2D.h"
#include "CameraMod_Fly.h"
#include "DREditorViewportClient.h"

FCameraManager::FCameraManager()
	: CurrentMod(ECameraMod::CameraNone)
	, FOVAngle(60.0f)
	, ViewportClient(nullptr)
	, bViewProject(true)
	, NearClipPlane(10.0f)
	, FarClipPlane(100000.0f)
	, OrthoScale(1.0f)
{
}

void FCameraManager::SetCameraMod(ECameraMod Mod)
{
	FCameraMod *OldMod = GetCurrentMod();
	if (OldMod)
	{
		OldMod->OnLeave();
	}

	CurrentMod = Mod;

	FCameraMod *CameraMod = GetCurrentMod();
	if (CameraMod)
	{
		CameraMod->OnEnter();
	}
}

ECameraMod FCameraManager::GetCurrentModType()
{
	return CurrentMod;
}

FCameraMod *FCameraManager::GetCurrentMod()
{
	int32 FoundIndex = INDEX_NONE;

	for (int32 i = 0; i < CameraMods.Num(); ++i)
	{
		if (CameraMods[i]->GetMod() == CurrentMod)
		{
			FoundIndex = i;
			break;
		}
	}

	return (FoundIndex!=INDEX_NONE)? CameraMods[FoundIndex] : nullptr;
}

FVector FCameraManager::GetViewRight()
{ 
	FVector ToDir = ViewRotation.Vector();
	FVector Right = FVector::CrossProduct(FVector::UpVector, ToDir);
	Right = Right.GetSafeNormal();
	return Right;
}

FVector	FCameraManager::GetViewForward()
{
	return ViewRotation.Vector();
}

FVector FCameraManager::GetViewUp()
{ 
	FVector ToDir = ViewRotation.Vector();
	FVector Right = FVector::CrossProduct(FVector::UpVector, ToDir);
	return FVector::CrossProduct(ToDir, Right);
}

void FCameraManager::GetViewPoint(FVector &out_CamLoc, FRotator &out_CamRotation)
{
	out_CamLoc = ViewLocation;
	out_CamRotation = ViewRotation;
}

FVector FCameraManager::UpdateViewLcation(const FVector &DeltaMove)
{
	ViewLocation = ViewLocation + DeltaMove*CameraConfig.MoveSpeed;
	return ViewLocation;
}

FRotator FCameraManager::UpdateViewRotation(const FVector &DeltaMove)
{
	FRotator Rotation = GetViewRotation();
	Rotation.Yaw += DeltaMove.X * CameraConfig.ViewYawSpeed;
	Rotation.Pitch += DeltaMove.Y * CameraConfig.ViewPitchSpeed;;
	Rotation.Pitch = FMath::Clamp<float>(Rotation.Pitch, -89.0f, 89.0f);
	SetViewRotation(Rotation);
	return Rotation;
}

void FCameraManager::GetMinimalViewInfo(FMinimalViewInfo &MinimalViewInfo)
{
	MinimalViewInfo.FOV = FOVAngle;
	MinimalViewInfo.DesiredFOV = FOVAngle;
	MinimalViewInfo.bConstrainAspectRatio = false;
	MinimalViewInfo.Location = ViewLocation;
	MinimalViewInfo.Rotation = ViewRotation;
	MinimalViewInfo.ProjectionMode = bViewProject ? ECameraProjectionMode::Perspective : ECameraProjectionMode::Orthographic;
	MinimalViewInfo.OrthoNearClipPlane = NearClipPlane;
	MinimalViewInfo.OrthoFarClipPlane = FarClipPlane;
}

void FCameraManager::SetViewportClient(FDREditorViewportClient *InViewportClient)
{
	ViewportClient = InViewportClient;

	if (CameraMods.Num() <= 0)
	{
		CameraMods.Add(new FCameraMod_Fly());
		CameraMods.Add(new FCameraMod_2D());
	
		for (int32 i = 0; i < CameraMods.Num(); ++i)
		{
			CameraMods[i]->SetViewportClient(ViewportClient);
		}
	}
}

void FCameraManager::SetOrthoScale(float InScale)
{
	OrthoScale = InScale;
	if (ViewportClient)
	{
		ASceneManager *SceneMgr = ViewportClient->GetSceneManager();
		if (SceneMgr)
		{
			SceneMgr->SetActorScale3D(FVector(InScale));
		}
	}
}



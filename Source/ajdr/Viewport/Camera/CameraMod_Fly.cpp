
#include "ajdr.h"
#include "CameraMod_Fly.h"
#include "DREditorViewportClient.h"

void FCameraMod_Fly::Reset()
{
	ViewportClient->CameraMgr.SetViewType(true);
	ViewportClient->CameraMgr.SetViewLocation(FVector(1661.0f, -6064.0f, 1437.5f));
	ViewportClient->CameraMgr.SetViewRotation(FRotator(-26.9f, 70.1f, 0));
	ViewportClient->SetWireframe(false);
	ViewportClient->CameraMgr.SetOrthoScale(1.0f);
}

bool FCameraMod_Fly::InputKey(FKey Key, EInputEvent Event)
{
	if (Key == EKeys::W || Key == EKeys::S || Key == EKeys::A || Key == EKeys::D)
	{
		FVector DeltaMove = FVector::ZeroVector;

		FVector Forward = ViewportClient->CameraMgr.GetViewForward();
		FVector Right =  ViewportClient->CameraMgr.GetViewRight();

		if (Key == EKeys::W)
		{
			DeltaMove = Forward;
		}
		else if (Key == EKeys::S)
		{
			DeltaMove = -Forward;
		}
		else if (Key == EKeys::A)
		{
			DeltaMove = -Right;
		}
		else if (Key == EKeys::D)
		{
			DeltaMove = Right;
		}

		ViewportClient->CameraMgr.UpdateViewLcation(DeltaMove);

		return true;
	}

	return false;
}

bool FCameraMod_Fly::InputAxis(FKey Key, float Delta, float DeltaTime)
{
	const bool bMouseButtonDown = ViewportClient->IsKeyDown(EKeys::LeftMouseButton) || ViewportClient->IsKeyDown(EKeys::RightMouseButton);

	const float DragX = (Key == EKeys::MouseX) ? Delta : 0.f;
	const float DragY = (Key == EKeys::MouseY) ? Delta : 0.f;

	if (bMouseButtonDown)
	{
		ViewportClient->CameraMgr.UpdateViewRotation(FVector(DragX, DragY, 0));
	}

	const bool bMiddleButtonDown = ViewportClient->IsKeyDown(EKeys::MiddleMouseButton);
	if (bMiddleButtonDown)
	{
		FVector Up = ViewportClient->CameraMgr.GetViewUp();
		FVector Right = ViewportClient->CameraMgr.GetViewRight();
		FVector DeltaMove = Up * DragY + Right*DragX;
		ViewportClient->CameraMgr.UpdateViewLcation(-0.1f*DeltaMove);
	}

	if (Key== EKeys::MouseWheelAxis)
	{
		static float WheelSpeed = 20.0f;
		FVector Forward = ViewportClient->CameraMgr.GetViewRotation().Vector();
		FVector DeltaMove = Forward * WheelSpeed * Delta;
		ViewportClient->CameraMgr.UpdateViewLcation(DeltaMove);
	}

	return false;
}




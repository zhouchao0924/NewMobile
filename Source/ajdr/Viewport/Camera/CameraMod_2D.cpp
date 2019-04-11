
#include "ajdr.h"
#include "CameraMod_2D.h"
#include "DREditorViewportWidget.h"
#include "DREditorViewportClient.h"

void FCameraMod_2D::Reset()
{
	ViewportClient->CameraMgr.SetViewType(true);
	ViewportClient->CameraMgr.SetViewLocation(FVector(0, 0, 1000.0f));
	FRotator Rotation = FVector(0, 0, -1.0f).Rotation();
	ViewportClient->CameraMgr.SetViewRotation(Rotation);
	ViewportClient->CameraMgr.SetFOVAngle(45.0f);
	ViewportClient->CameraMgr.SetOrthoScale(0.25f);
}

bool FCameraMod_2D::InputKey(FKey Key, EInputEvent Event)
{
	if ( Key == EKeys::LeftMouseButton &&
		 ViewportClient->IsKeyDown(EKeys::SpaceBar))
	{
		OriginalMousePosition = ViewportClient->GetCursorPosition();
		OriginalViewLocation = ViewportClient->CameraMgr.GetViewLocation();
		return true;
	}
	return false;
}                                       

bool FCameraMod_2D::InputAxis(FKey Key, float Delta, float DeltaTime)
{
	SDRViewport *ViewportSlate = ViewportClient->GetViewportSlate();
	if (ViewportSlate)
	{
		if (ViewportClient->IsKeyDown(EKeys::SpaceBar))
		{
			const float DragX = (Key == EKeys::MouseX) ? Delta : 0.f;
			const float DragY = (Key == EKeys::MouseY) ? Delta : 0.f;

			if (ViewportClient->IsKeyDown(EKeys::LeftMouseButton))
			{
				FVector2D CurrentLoc = ViewportClient->GetCursorPosition();
				FVector2D DeltaMove = CurrentLoc - OriginalMousePosition;
   				Move2D(DeltaMove);
			}

			if (Key == EKeys::MouseWheelAxis)
			{
				Scale2D(Delta);
			}

			return true;
		}	
	}

	return false;
}

void FCameraMod_2D::Move2D(const FVector2D &MoveDelta)
{
	float OldScale = ViewportClient->CameraMgr.GetOrthoScale();
	FVector OldTrans = ViewportClient->CameraMgr.GetViewLocation();	
	FVector2D ScaledDelta = MoveDelta * 1.0/OldScale;
	FVector NewViewLocation = OriginalViewLocation + FVector(ScaledDelta.Y, -ScaledDelta.X, 0);
	ViewportClient->CameraMgr.SetViewLocation(NewViewLocation);
}

void FCameraMod_2D::Scale2D(float Delta)
{ 
	float		OldScale = ViewportClient->CameraMgr.GetOrthoScale();
	FVector2D   CurCursorPos = ViewportClient->GetCursorPosition();
	FVector		CurWorldPos = ViewportClient->GetWorldPositionAtCursor();

	float fScale = OldScale*(1.0f - 0.08f*Delta);
	ViewportClient->CameraMgr.SetOrthoScale(fScale);
}


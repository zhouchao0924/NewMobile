

#include "ajdr.h"
#include "EditMode.h"
#include "Viewport/DRDrawer.h"
#include "Operation_DrawWall.h"
#include "UnrealMathUtility.h"

FWallSnapConfig::FWallSnapConfig()
	: bTerminal(true)
	, bCenter(true)
	, bHorizontalOrVertical(true)
	, Radius(40.0f)
	, bAuxLine(true)
	, bDrawAllSnapPoint(true)
	, bDrawNormal(true)
{
}

/////////////////////////////////////////////////////////////////////////////////////
UOperation_DrawWall::UOperation_DrawWall(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, LastPoint(INDEX_NONE)
{
}

void UOperation_DrawWall::InputKey(FViewport *Viewport, FKey Key, EInputEvent Event)
{
	HandleDrawWall(Viewport, Key, Event);

	if (Key == EKeys::Escape)
	{
		EditMode->CancleOperation();
	}
}

void UOperation_DrawWall::InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime)
{
}

void UOperation_DrawWall::DrawSnapLoc(FDrawer *Drawer)
{
	if (_SnapPoint.bActive)
	{
		TArray<FVector2D> Lines;
		Lines.SetNum(5);
		/*
		   1
		0     2
		   3
		*/
		const float Size = 20.0f;
		Lines[0] = _SnapPoint.Loc + FVector2D(-Size, 0);
		Lines[1] = _SnapPoint.Loc + FVector2D(0, Size);
		Lines[2] = _SnapPoint.Loc + FVector2D(Size, 0);
		Lines[3] = _SnapPoint.Loc + FVector2D(0, -Size);
		Lines[4] = Lines[0];

		Drawer->DrawLine(Lines.GetData(), Lines.Num(), FLinearColor::Green);
	}
}

void UOperation_DrawWall::DrawAuxAxis(FDrawer *Drawer)
{
	if (_AuxAxisArray.Num()>0)
	{
		for (int I = 0; I < _AuxAxisArray.Num(); ++I)
		{
			FAuxAxis &AuxAxis = _AuxAxisArray[I];
			Drawer->DrawDotLine(AuxAxis.Start, AuxAxis.End, FLinearColor(1.0f, 1.0f, 1.0f, 0.5f));
		}
	}
}

void UOperation_DrawWall::HandleDrawWall(FViewport *Viewport, FKey Key, EInputEvent Event)
{
	UBuildingSystem *BuildingSystem = GetBuildingSystem();
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if ( Key == EKeys::LeftMouseButton &&  
		 Event == EInputEvent::IE_Pressed)
	{
		int32 NewPointIndex = INDEX_NONE;

		if (_SnapPoint.bActive)
		{  
			if (_SnapPoint.BestID != INDEX_NONE)
			{
				NewPointIndex = _SnapPoint.BestID;
			}
			else
			{
				NewPointIndex = BuildingSystem->AddCorner(_SnapPoint.Loc);
			}
		}
		else
		{
			NewPointIndex = BuildingSystem->AddCorner(_CurrentPosition);
		}

		if (LastPoint != INDEX_NONE)
		{
			BuildingSystem->AddWall(LastPoint, NewPointIndex);
		}

		LastPoint = NewPointIndex;
	}
	else if( Key==EKeys::RightMouseButton && 
			 Event==EInputEvent::IE_Pressed)
	{
		if (LastPoint != INDEX_NONE && BuildingSystem->IsFree(LastPoint))
		{
			BuildingSystem->DeleteObject(LastPoint);
		}
		LastPoint = INDEX_NONE;
		EditMode->CancleOperation();
	}
}

void UOperation_DrawWall::Draw2D(FDrawer *Drawer)
{
	UBuildingSystem *BuildingSystem = GetBuildingSystem();
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if (ViewportClient && BuildingSystem)
	{
		FVector WorldPos = ViewportClient->GetWorldPositionAtCursor();
		if (LastPoint != INDEX_NONE)
		{
			UBuildingData *Data = BuildingSystem->GetData(LastPoint);
			IObject *pObj = Data? Data->GetRawObj() : nullptr;
			if (pObj)
			{
				FVector2D Loc = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
				Drawer->DrawLine(Loc, FVector2D(WorldPos.X, WorldPos.Y), FLinearColor::White);
			}
		}

		HandleWallSnap();
		DrawSnapLoc(Drawer);
		DrawAuxAxis(Drawer);
	}
}

void UOperation_DrawWall::CaclWorldPosition()
{
	FDREditorViewportClient *ViewportClient = GetViewportClient();
	if (ViewportClient)
	{
		FVector CurPos = ViewportClient->GetWorldPositionAtCursor();
		_CurrentPosition = FVector2D(CurPos.X, CurPos.Y);
	}
}

void UOperation_DrawWall::HandleWallSnap()
{
	CaclWorldPosition();

	FVector2D	BestLoc;
	int32		BestID = INDEX_NONE;
	FVector2D	UIMin(-9999.9f, -9999.9f);
	FVector2D	UIMax( 9999.9f,  9999.9f);

	_SnapPoint.Reset();
	_AuxAxisArray.Empty();

	UBuildingSystem *BuildingSystem = GetBuildingSystem();
	if (BuildingSystem)
	{
		int32 Pt = BuildingSystem->Snap(_CurrentPosition, LastPoint, BestLoc, BestID, Config.Radius);
	
		if (Pt != kESnapType::kENone)
		{
			if (Pt == kESnapType::kEDirX)
			{
				FAuxAxis AuxAxis;
				AuxAxis.Start = FVector2D(BestLoc.X, UIMin.Y);
				AuxAxis.End = FVector2D(BestLoc.X, UIMax.Y);
				_AuxAxisArray.Add(AuxAxis);
			}
			else if (Pt == kESnapType::kEDirY)
			{
				FAuxAxis AuxAxis;
				AuxAxis.Start = FVector2D(UIMin.X, BestLoc.Y);
				AuxAxis.End = FVector2D(UIMax.X, BestLoc.Y);
				_AuxAxisArray.Add(AuxAxis);
			}
			else if (Pt == kESnapType::kEPt)
			{
				_SnapPoint.bActive = true;
				_SnapPoint.BestID = BestID;
				_SnapPoint.Loc = BestLoc;
			}
		}

		if (LastPoint != INDEX_NONE && Pt != kESnapType::kEPt)
		{
			IObject *pCorner = BuildingSystem->GetObject(LastPoint);
			if (pCorner)
			{
				FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
				FVector2D Vec = _CurrentPosition - Location;
				if (std::abs(Vec.X) < Config.Radius)
				{
					FAuxAxis AuxAxis;
					AuxAxis.Start = Location;
					AuxAxis.End = FVector2D(Location.X, (Vec.Y < 0 ? UIMin.Y: UIMax.Y));
					_AuxAxisArray.Add(AuxAxis);
				}
				if (std::abs(Vec.Y) < Config.Radius)
				{
					FAuxAxis AuxAxis;
					AuxAxis.Start = Location;
					AuxAxis.End = FVector2D((Vec.X < 0 ? UIMin.X : UIMax.X), Location.Y);
					_AuxAxisArray.Add(AuxAxis);
				}
			}
		}

		if (_AuxAxisArray.Num()>0)
		{
			bool bIntersec = false;
			_SnapPoint.bActive = true;

			if (_AuxAxisArray.Num() > 1)
			{
				FAuxAxis &Line0 = _AuxAxisArray[0];

				for (INT I = 1; I < _AuxAxisArray.Num(); ++I)
				{
					FAuxAxis &Line1 = _AuxAxisArray[I];
					FVector IntersecPos;
					if (FMath::SegmentIntersection2D(FVector(Line0.Start, 0), FVector(Line0.End, 0), FVector(Line1.Start, 0), FVector(Line1.End, 0), IntersecPos))
					{
						bIntersec = true;
						_SnapPoint.Loc = FVector2D(IntersecPos);
						break;
					}
				}
			}

			if (!bIntersec)
			{
				FVector2D ProjLoc;
				float Dist = std::numeric_limits<float>::infinity();

				for (INT I = 0; I < _AuxAxisArray.Num(); ++I)
				{
					FAuxAxis &AuxAxis = _AuxAxisArray[I];
					FVector ProjD = FMath::ClosestPointOnSegment(FVector(_CurrentPosition, 0), FVector(AuxAxis.Start, 0), FVector(AuxAxis.End, 0));
					float D = (ProjD-FVector(_CurrentPosition,0)).SizeSquared();
					if (D < Dist)
					{
						Dist = D;
						ProjLoc = FVector2D(ProjD.X, ProjD.Y);
					}
				}

				_SnapPoint.Loc = ProjLoc;
			}
		}
	}
}





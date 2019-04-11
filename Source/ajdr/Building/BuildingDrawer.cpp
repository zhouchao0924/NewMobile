
#include "ajdr.h"
#include "Math/kVector2D.h"
#include "BuildingDrawer.h"
#include "Building.h"
#include "DRHitProxy.h"

float FBuildingDrawer::WallThickness = 30.0f;

void FBuildingDrawer::BeginObject(FDrawer *Drawer, IObject *Obj)
{
	if (Drawer && Obj)
	{
		Drawer->SetHitProxy(new HBuildingObject(Obj->GetID()));
	}
}

void FBuildingDrawer::EndObject(FDrawer *Drawer)
{
	if (Drawer)
	{
		Drawer->SetHitProxy(nullptr);
	}
}

void FBuildingDrawer::DrawWall(FDrawer *Drawer, IObject *pWall, bool bSelected)
{	
	BeginObject(Drawer, pWall);

	ISuite *Suite = pWall->GetSuite();

	kLine c_line, l_line, r_line;
	Suite->GetWallBorderLines(pWall->GetID(), FORCE_TYPE(kLine, c_line), FORCE_TYPE(kLine, l_line), FORCE_TYPE(kLine, r_line));

	/*  2---------------3
	  1					  4
		0---------------5
	*/
	FVector2D RP0 = FVector2D(r_line.start.x, r_line.start.y);
	FVector2D RP1 = FVector2D(r_line.end.x, r_line.end.y);

	FVector2D LP0 = FVector2D(l_line.start.x, l_line.start.y);
	FVector2D LP1 = FVector2D(l_line.end.x, l_line.end.y);

	FVector2D P0 = FVector2D(c_line.start.x, c_line.start.y);
	FVector2D P1 = FVector2D(c_line.end.x, c_line.end.y);

	FVector2D Right = ToVector2D(pWall->GetPropertyValue("Right").Vec2Value());
	FVector2D Forward = (P1 - P0).GetSafeNormal();

	IValue &Holes = pWall->GetPropertyValue("Holes");
	int nHoles = Holes.GetNumFields();
	if (nHoles > 0)
	{
		FVector2D R = RP0, L = LP0;

		for (INT I = 0; I < nHoles; ++I)
		{
			IValue &Hole = Holes.GetField(I);

			float MinX = Hole.GetField("MinX").FloatValue();
			float MaxX = Hole.GetField("MaxX").FloatValue();

			FVector2D P = P0 + Forward * MinX;
			FVector2D EL = P - Right * pWall->GetPropertyValue("ThickLeft").FloatValue();
			FVector2D ER = P + Right * pWall->GetPropertyValue("ThickRight").FloatValue();

			Drawer->DrawLine(R, ER, GetColor(bSelected));
			Drawer->DrawLine(L, EL, GetColor(bSelected));
			Drawer->DrawLine(EL, ER, GetColor(bSelected));

			L = EL + Forward * (MaxX - MinX);
			R = ER + Forward * (MaxX - MinX);

			Drawer->DrawLine(L, R, GetColor(bSelected));
		}

		Drawer->DrawLine(L, LP1, GetColor(bSelected));
		Drawer->DrawLine(R, RP1, GetColor(bSelected));
	}
	else
	{
		Drawer->DrawLine(RP0, RP1, GetColor(bSelected));
		Drawer->DrawLine(LP0, LP1, GetColor(bSelected));
	}

	EndObject(Drawer);
}

void FBuildingDrawer::DrawCorner(FDrawer *Drawer, IObject *pCorner)
{

}

void FBuildingDrawer::DrawRoom(FDrawer *Drawer, IObject *pRoom)
{

}

void FBuildingDrawer::DrawDoor(FDrawer *Drawer, IObject *pDoor, bool bSelected)
{
	if (pDoor)
	{
		BeginObject(Drawer, pDoor);

		ISuite *Suite = pDoor->GetSuite();
		int32 CornerID = pDoor->GetPropertyValue("CornerID").IntValue();
		int32 WallID = pDoor->GetPropertyValue("WallID").IntValue();

		IObject *pCorner = Suite->GetObject(CornerID, ECorner);
		IObject *pWall = Suite->GetObject(WallID, EWall);

		FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
		FVector2D Forward = ToVector2D(pWall->GetPropertyValue("Forward").Vec2Value());
		FVector2D Right = ToVector2D(pWall->GetPropertyValue("Right").Vec2Value());
		float Width = pDoor->GetPropertyValue("Width").FloatValue();

		DrawDoor(Drawer, bSelected, Location, Forward, -Right, Width);
		
		EndObject(Drawer);
	}
}

void FBuildingDrawer::DrawDoor(FDrawer *Drawer, bool bSelected, const FVector2D &Loc, const FVector2D &Forward, const FVector2D &Right, float Width)
{
	float Radius = Width / 2.0f;
	Drawer->DrawLine(Loc - Forward * Radius, Loc + Forward * Radius, GetColor(bSelected));
	Drawer->DrawLine(Loc - Forward * Radius, Loc - Forward * Radius + Right * Radius, GetColor(bSelected));
	Drawer->DrawLine(Loc + Forward * Radius, Loc + Forward * Radius + Right * Radius, GetColor(bSelected));
	Drawer->DrawArc(Loc - Forward * Radius, Forward, Right, Radius, 0, FMath::DegreesToRadians(90.0f), 20, GetColor(bSelected));
	Drawer->DrawArc(Loc + Forward * Radius, Forward, Right, Radius, FMath::DegreesToRadians(90.0f), PI, 20, GetColor(bSelected));
}

void FBuildingDrawer::DrawWindow(FDrawer *Drawer, IObject *pWindow, bool bSelected)
{
	if (pWindow)
	{
		BeginObject(Drawer, pWindow);

		ISuite *Suite = pWindow->GetSuite();
		int32 CornerID = pWindow->GetPropertyValue("CornerID").IntValue();
		int32 WallID = pWindow->GetPropertyValue("WallID").IntValue();

		IObject *pCorner = Suite->GetObject(CornerID, ECorner);
		IObject *pWall = Suite->GetObject(WallID, EWall);

		FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
		FVector2D Forward = ToVector2D(pWall->GetPropertyValue("Forward").Vec2Value());
		FVector2D Right = ToVector2D(pWall->GetPropertyValue("Right").Vec2Value());
		float Width = pWindow->GetPropertyValue("Width").FloatValue();

		DrawWindow(Drawer, bSelected, Location, Forward, -Right, Width);
		
		EndObject(Drawer);
	}
}

FLinearColor FBuildingDrawer::GetColor(bool bSelected)
{
	return bSelected ? FLinearColor::Green : FLinearColor::White;
}

void FBuildingDrawer::DrawWindow(FDrawer *Drawer, bool bSelected, const FVector2D &Loc, const FVector2D &Forward, const FVector2D &Right, float Width)
{
	FVector2D V[7];
	float HalfWidth = Width / 2.0f;

	V[0] = Loc - Forward * HalfWidth + Right * WallThickness;
	V[1] = Loc + Forward * HalfWidth + Right * WallThickness;
	V[2] = Loc + Forward * HalfWidth - Right * WallThickness;
	V[3] = Loc - Forward * HalfWidth - Right * WallThickness;
	V[4] = V[0];
	V[5] = Loc - Forward * HalfWidth;
	V[6] = Loc + Forward * HalfWidth;

	Drawer->DrawLine(V, 7, GetColor(bSelected));
}



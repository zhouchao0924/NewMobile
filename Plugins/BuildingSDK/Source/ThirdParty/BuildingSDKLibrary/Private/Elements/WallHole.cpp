
#include "Wall.h"
#include "Corner.h"
#include "ISuite.h"
#include "WallHole.h"
#include "ModelInstance.h"
#include "Class/Property.h"

WallHole::WallHole()
	: ZPos(0)
	, Width(0)
	, Height(0)
	, Thickness(0)
	, bVerticalFlip(false)
	, bHorizonalFlip(false)
{
}

BEGIN_DERIVED_CLASS(WallHole, Anchor)
	ADD_PROP_CHANNEL(ZPos, FloatProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(Width, FloatProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(Height, FloatProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(Thickness, FloatProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(bHorizonalFlip, BoolProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(bVerticalFlip, BoolProperty, EChannelGeometry)
END_CLASS()

bool WallHole::GetRange(float &MinX, float &MaxX)
{
	Wall *pWall = SUITE_GET_BUILDING_OBJ(OwnerID, Wall);
	if (pWall)
	{
		Corner *pCorner0 = SUITE_GET_BUILDING_OBJ(pWall->P[0], Corner);
		if (pCorner0)
		{
			kPoint Loc = GetCornerLocation();
			float Dist = (Loc - pCorner0->Location).Size();
			MinX = Dist - Width / 2.0f;
			MaxX = Dist + Width / 2.0f;
			return true;
		}
	}
	return false;
}

void WallHole::GetBorderLines(kLine &l_Left, kLine &l_Right)
{
	Wall *pWall = SUITE_GET_BUILDING_OBJ(OwnerID, Wall);
	if (pWall)
	{
		kPoint Right = pWall->GetRight();
		kPoint Forward = pWall->GetForward();

		kPoint Loc = GetCornerLocation();
		kPoint S = Loc - Forward * Width / 2.0f;
		kPoint E = Loc + Forward * Width / 2.0f;

		l_Left.start = S - Right * pWall->GetThickLeft();
		l_Left.end = E - Right * pWall->GetThickRight();

		l_Right.start = S + Right * pWall->GetThickLeft();
		l_Right.end = E + Right * pWall->GetThickRight();
	}
}

void WallHole::GetPolygon(kPoint *&pPolygon, int &Num)
{
	if (polygons.size() <= 0)
	{
		kPoint P0, P1;
		Corner *pCorner = SUITE_GET_BUILDING_OBJ(GetCorner(0), Corner);
		Wall *pWall = SUITE_GET_BUILDING_OBJ(OwnerID, Wall);
		pWall->GetLocations(P0, P1);
		float OffsetX = (pCorner->Location - P0).Size();
		float HalfWidth = Width / 2.0f;
		polygons.push_back(kPoint(OffsetX - HalfWidth, ZPos));
		polygons.push_back(kPoint(OffsetX - HalfWidth, ZPos + Height));
		polygons.push_back(kPoint(OffsetX + HalfWidth, ZPos + Height));
		polygons.push_back(kPoint(OffsetX + HalfWidth, ZPos));
	}
	pPolygon = &polygons[0];
	Num = (int)polygons.size();
}

IValue *WallHole::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = Anchor::GetFunctionProperty(name);
	if (!pValue)
	{
		if (name == "WallID")
		{
			pValue = &GValueFactory->Create(OwnerID);
		}
	}
	return pValue;
}

void WallHole::UpdateTransform(ModelInstance *pModel)
{
	Corner *pCorner = SUITE_GET_BUILDING_OBJ(GetCorner(0), Corner);
	if (pCorner)
	{
		pModel->Size.X = Width;
		pModel->Size.Z = Height;
		pModel->Location = kVector3D(pCorner->Location.X, pCorner->Location.Y, ZPos);
	}

	Wall *pWall = SUITE_GET_BUILDING_OBJ(OwnerID, Wall);
	if (pWall)
	{
		kPoint Forward = pWall->GetForward();
		pModel->Forward = bHorizonalFlip? -Forward : Forward;
		pWall->UpdateHole(this);
	}

	polygons.clear();
	pModel->AlignType = EAlignBottomCenter;
}

void WallHole::OnDestroy()
{
	Wall *pWall = SUITE_GET_BUILDING_OBJ(OwnerID, Wall);
	if (pWall)
	{
		pWall->MarkNeedUpdate();
	}
	Anchor::OnDestroy();
}



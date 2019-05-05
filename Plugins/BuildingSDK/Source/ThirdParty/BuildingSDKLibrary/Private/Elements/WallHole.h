
#pragma once


#include "kLine.h"
#include "Anchor.h"
#include "Math/kVector2D.h"

class WallHole :public  Anchor
{
	DEFIN_CLASS()
public:
	WallHole();
	bool IsDeletable() override { return true; }
	void OnDestroy() override;
	EObjectType GetType() { return EWallHole; }
	bool GetRange(float &MinX, float &MaxX);
	void GetPolygon(kPoint *&pPolygon, int &Num);
	void GetBorderLines(kLine &l_Left, kLine &l_Right);
	void UpdateTransform(ModelInstance *pModel) override;
	IValue *GetFunctionProperty(const std::string &name) override;
public:
	float	 ZPos;
	float	 Width;
	float	 Height;
	float	 Thickness;
	bool	 bHorizonalFlip;
	bool	 bVerticalFlip;
	std::vector<kPoint> polygons;
};



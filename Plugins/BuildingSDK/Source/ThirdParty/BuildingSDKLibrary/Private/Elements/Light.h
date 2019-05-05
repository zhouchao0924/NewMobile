
#pragma once

#include "kVector3D.h"
#include "BuildingObject.h"

class Light :public BuildingObject
{
	DEFIN_CLASS()
public:
	Light();
	EObjectType GetType() { return ELight; }
public:
	kColor		LightColor;
	bool		IsCastShadow;
	bool		IsVisible;
};



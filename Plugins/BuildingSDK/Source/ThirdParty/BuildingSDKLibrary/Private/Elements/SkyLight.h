
#pragma once

#include "Light.h"

class SkyLight :public Light
{
	DEFIN_CLASS()
public:
	SkyLight();
	EObjectType GetType() { return ESkyLight; }
public:
	kRotation	DirectionLightRotation;
	kColor		DirectionLightColor;
	float		DirectionLightIntensity;
	float		SkyLightIntensity;
	kColor		SkyLightColor;
	float		SkyLightAngle;
};



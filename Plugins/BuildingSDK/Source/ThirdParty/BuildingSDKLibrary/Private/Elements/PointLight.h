
#pragma once

#include "Light.h"

class PointLight :public Light
{
	DEFIN_CLASS()
public:
	PointLight();
	EObjectType GetType() { return EPointLight; }
public:
	bool		IsAutoPoint;
	kVector3D	Location;
	float		SoftSourceRadius;
	float		SourceRadius;
	float		SourceLength;
	float		Intensity;
	float		AttenuationRadius;
	float		ShadowResolutionScale;
};



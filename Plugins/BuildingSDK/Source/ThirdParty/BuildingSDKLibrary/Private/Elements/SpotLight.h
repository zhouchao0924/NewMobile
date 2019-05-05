
#pragma once

#include "Light.h"

class SpotLight :public Light
{
	DEFIN_CLASS()
public:
	SpotLight();
	EObjectType GetType() { return ESpotLight; }
public:
	float		InnerConeAngle;
	float		OuterConeAngle;
	kVector3D	Location;
	kRotation	Rotation;
	float		SoftSourceRadius;
	float		SourceRadius;
	float		SourceLength;
	float		Intensity;
	float		AttenuationRadius;
	float		ShadowResolutionScale;
};



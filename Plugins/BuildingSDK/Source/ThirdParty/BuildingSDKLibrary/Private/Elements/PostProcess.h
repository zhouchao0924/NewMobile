
#pragma once

#include "kVector3D.h"
#include "BuildingObject.h"

class PostProcess :public BuildingObject
{
	DEFIN_CLASS()
public:
	PostProcess();
	EObjectType GetType() { return EPostProcess; }
public:
	float Saturation;
	float Constrast;
	float BloomIntensity;
	float AmbientOcclusion;
	float AmbientOcclusionRadius;
};





#include "ISuite.h"
#include "SkyLight.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(SkyLight, Light)
	ADD_PROP(DirectionLightRotation, RotatorProperty)
	ADD_PROP(DirectionLightColor, ColorProperty)
	ADD_PROP(DirectionLightIntensity, FloatProperty)
	ADD_PROP(SkyLightIntensity, FloatProperty)
	ADD_PROP(SkyLightColor, ColorProperty)
	ADD_PROP(SkyLightAngle, FloatProperty)
END_CLASS()

SkyLight::SkyLight()
	: DirectionLightColor(255,255,255,255)
	, DirectionLightRotation(0,24.0f,-14.0f)
	, DirectionLightIntensity(1.0f)
	, SkyLightIntensity(1.4f)
	, SkyLightColor(255, 255, 255, 255)
	, SkyLightAngle(57.0f)
{
}



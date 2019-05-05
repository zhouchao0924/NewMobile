

#include "ISuite.h"
#include "PointLight.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(PointLight, Light)
	ADD_PROP(IsAutoPoint, BoolProperty)
	ADD_PROP(Location, Vec3DProperty)
	ADD_PROP(SoftSourceRadius, FloatProperty)
	ADD_PROP(SourceRadius, FloatProperty)
	ADD_PROP(SourceLength, FloatProperty)
	ADD_PROP(Intensity, FloatProperty)
	ADD_PROP(AttenuationRadius, FloatProperty)
	ADD_PROP(ShadowResolutionScale, FloatProperty)
END_CLASS()

PointLight::PointLight()
	: IsAutoPoint(false)
	, Intensity(8.0f)
	, SourceRadius(0.0f)
	, SoftSourceRadius(0.0f)
	, SourceLength(0.0f)
	, AttenuationRadius(100.0f)
	, ShadowResolutionScale(0.5f)
{
}


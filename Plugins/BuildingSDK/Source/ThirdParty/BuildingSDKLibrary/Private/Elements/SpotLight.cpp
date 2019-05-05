

#include "ISuite.h"
#include "SpotLight.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(SpotLight, Light)
	ADD_PROP(Location, Vec3DProperty)
	ADD_PROP(Rotation, RotatorProperty)
	ADD_PROP(SoftSourceRadius, FloatProperty)
	ADD_PROP(SourceRadius, FloatProperty)
	ADD_PROP(SourceLength, FloatProperty)
	ADD_PROP(Intensity, FloatProperty)
	ADD_PROP(AttenuationRadius, FloatProperty)
	ADD_PROP(ShadowResolutionScale, FloatProperty)
	ADD_PROP(InnerConeAngle, FloatProperty)
	ADD_PROP(OuterConeAngle, FloatProperty)
END_CLASS()

SpotLight::SpotLight()
	: InnerConeAngle(45.0f)
	, OuterConeAngle(60.0f)
	, Intensity(8.0f)
	, SourceRadius(0.0f)
	, SoftSourceRadius(0.0f)
	, SourceLength(0.0f)
	, AttenuationRadius(100.0f)
	, ShadowResolutionScale(0.5f)
{
}


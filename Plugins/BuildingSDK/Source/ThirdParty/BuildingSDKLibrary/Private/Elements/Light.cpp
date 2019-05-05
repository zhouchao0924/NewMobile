

#include "ISuite.h"
#include "Light.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(Light, BuildingObject)
 	ADD_PROP(LightColor, ColorProperty)
	ADD_PROP(IsCastShadow, BoolProperty)
	ADD_PROP(IsVisible, BoolProperty)
END_CLASS()

Light::Light()
	: IsVisible(true)
	, IsCastShadow(false)
	, LightColor(255,255,255,255)
{
}


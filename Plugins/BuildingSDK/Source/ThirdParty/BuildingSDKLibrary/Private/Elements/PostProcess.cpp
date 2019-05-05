

#include "ISuite.h"
#include "PostProcess.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(PostProcess, BuildingObject)
 	ADD_PROP(Saturation, FloatProperty)
	ADD_PROP(Constrast, FloatProperty)
	ADD_PROP(BloomIntensity, FloatProperty)
	ADD_PROP(AmbientOcclusion, FloatProperty)
	ADD_PROP(AmbientOcclusionRadius, FloatProperty)
END_CLASS()

PostProcess::PostProcess()
	: Saturation(1.0f)
	, Constrast(1.0f)
	, BloomIntensity(1.0f)
	, AmbientOcclusion(0.5f)
	, AmbientOcclusionRadius(50.0f)
{
}


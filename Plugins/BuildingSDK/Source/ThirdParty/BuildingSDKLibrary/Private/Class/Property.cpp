
#include "Property.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"

Property::Property(int InOffset, EChannelMask InMask) 
	: ChannelMask(InMask)
	, Offset(InOffset)
	, bReadOnly(true)
	, Desc(nullptr)
{
}

IObjectDesc *Property::GetDesc()
{ 
	return Desc; 
}

void Property::SetName(const char *InName)
{
	PropName = InName;
}

void Property::SetValue(void *v, const IValue *value)
{
	OnSetValue(v, value);
	OnAfterSetValue((IObject *)v);
}

void Property::OnAfterSetValue(IObject *pObj)
{
	if (ChannelMask!=EChannelNone)
	{
		pObj->MarkNeedUpdate(ChannelMask);
	}
}

IntProperty::IntProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * IntProperty::GetValue(void *v)
{
	if (v)
	{
		int *intV = (int *)(((char *)v + Offset));
		return &GValueFactory->Create(*intV);
	}
	return nullptr;
}

void IntProperty::OnSetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_Int)
	{
		int *intV = (int *)(((char *)v + Offset));
		*intV = value->IntValue();
	}
}

IntArrayProperty::IntArrayProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * IntArrayProperty::GetValue(void *v)
{
	if (v)
	{
		std::vector<int> *intArray = (std::vector<int> *)(((char *)v + Offset));
		return &GValueFactory->Create(kArray<int>(*intArray));
	}
	return nullptr;
}

void IntArrayProperty::OnSetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_IntArray)
	{
		std::vector<int> *intArray = (std::vector<int> *)(((char *)v + Offset));
		value->IntArrayValue().saveto(*intArray);
	}
}

Vec2DArrayProperty::Vec2DArrayProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue *Vec2DArrayProperty::GetValue(void *v)
{
	if (v)
	{
		std::vector<kPoint> *vec2Array = (std::vector<kPoint> *)(((char *)v + Offset));
		return &GValueFactory->Create(kArray<kPoint>(*vec2Array));
	}
	return nullptr;
}

void Vec2DArrayProperty::OnSetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_Vec2DArray)
	{
		std::vector<kPoint> *vec2Array = (std::vector<kPoint> *)(((char *)v + Offset));
		value->Vec2ArrayValue().saveto(*vec2Array);
	}
}

BoolProperty::BoolProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * BoolProperty::GetValue(void *v)
{
	if (v)
	{
		bool *bV = (bool *)(((char *)v + Offset));
		return &GValueFactory->Create(*bV);
	}
	return nullptr;
}

void BoolProperty::OnSetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_Bool)
	{
		bool *bV = (bool *)(((char *)v + Offset));
		*bV = value->BoolValue();
	}
}

FloatProperty::FloatProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * FloatProperty::GetValue(void *obj)
{
	if (obj)
	{
		float *fV = (float *)(((char *)obj + Offset));
		return &GValueFactory->Create(*fV);
	}
	return nullptr;
}

void FloatProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Float)
	{
		float *fV = (float *)(((char *)obj + Offset));
		*fV = value->FloatValue();
	}
}

RawStringProperty::RawStringProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * RawStringProperty::GetValue(void *obj)
{
	if (obj)
	{
		const char *strV = (const char *)(((char *)obj + Offset));
		return &GValueFactory->Create(strV);
	}
	return nullptr;
}

StdStringProperty::StdStringProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * StdStringProperty::GetValue(void *obj)
{
	if (obj)
	{
		std::string *strV = (std::string *)(((char *)obj + Offset));
		return &GValueFactory->Create(strV);
	}
	return nullptr;
}

void StdStringProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_StdString || value->GetType() ==kV_RawString)
	{
		std::string *strV = (std::string *)(((char *)obj + Offset));
		*strV = value->StrValue();
	}
}

Vec3DProperty::Vec3DProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * Vec3DProperty::GetValue(void *obj)
{
	if (obj)
	{
		kVector3D *vec3D = (kVector3D *)(((char *)obj + Offset));
		return &GValueFactory->Create(vec3D);
	}
	return nullptr;
}

void Vec3DProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Vec3D)
	{
		kVector3D *vec3D = (kVector3D *)(((char *)obj + Offset));
		*vec3D = value->Vec3Value();
	}
}

RotatorProperty::RotatorProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * RotatorProperty::GetValue(void *obj)
{
	if (obj)
	{
		kRotation *vec3D = (kRotation *)(((char *)obj + Offset));
		return &GValueFactory->Create(vec3D);
	}
	return nullptr;
}

void RotatorProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Rotator)
	{
		kRotation *rotation = (kRotation *)(((char *)obj + Offset));
		*rotation = value->RotationValue();
	}
}

Vec2DProperty::Vec2DProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * Vec2DProperty::GetValue(void *obj)
{
	if (obj)
	{
		kPoint *vec2D = (kPoint *)(((char *)obj + Offset));
		return &GValueFactory->Create(vec2D);
	}
	return nullptr;
}

void Vec2DProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Vec2D)
	{
		kPoint *vec2D = (kPoint *)(((char *)obj + Offset));
		*vec2D = value->Vec2Value();
	}
}

BoxProperty::BoxProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * BoxProperty::GetValue(void *obj)
{
	if (obj)
	{
		kBox3D *box = (kBox3D *)(((char *)obj + Offset));
		return &GValueFactory->Create(box);
	}
	return nullptr;
}

void BoxProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Bounds)
	{
		kBox3D *box = (kBox3D *)(((char *)obj + Offset));
		*box = value->Bounds();
	}
}

PlaneProperty::PlaneProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue * PlaneProperty::GetValue(void *obj)
{
	if (obj)
	{
		kPlane3D *plane = (kPlane3D *)(((char *)obj + Offset));
		return &GValueFactory->Create(plane);
	}
	return nullptr;
}

void PlaneProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Plane)
	{
		kPlane3D *plane = (kPlane3D *)(((char *)obj + Offset));
		*plane = value->PlaneValue();
	}
}

ColorProperty::ColorProperty(int InOffset, EChannelMask InMask)
	:Property(InOffset, InMask)
{
}

IValue *ColorProperty::GetValue(void *obj)
{
	if (obj)
	{
		kColor *color = (kColor *)(((char *)obj + Offset));
		return &GValueFactory->Create(*color);
	}
	return nullptr;
}

void ColorProperty::OnSetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Color)
	{
		kColor *color = (kColor *)(((char *)obj + Offset));
		*color = value->ColorValue();
	}
}



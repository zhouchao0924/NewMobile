
#pragma once

#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kBox.h"
#include "Math/kPlane.h"
#include "IProperty.h"
#include "ObjectDesc.h"

class Property :public IProperty
{
public:
	Property(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override{ return kV_Nil; }
	IValue *GetValue(void *v) override { return nullptr; }
	void SetValue(void *v, const IValue *value) override;
	virtual void OnSetValue(void *v, const IValue *value)  = 0;
	const char *GetName() override{ return PropName.c_str(); }
	void SetName(const char *InName) override;
	bool IsReadOnly() override { return bReadOnly; }
	class IObjectDesc *GetDesc() override;
	int GetOffset() override { return Offset; }
	void OnAfterSetValue(IObject *pObj);
	friend class ObjectDesc;
protected:
	int				Offset;
	std::string		PropName;
	bool			bReadOnly;
	ObjectDesc		*Desc;
	EChannelMask	ChannelMask;
};

class IntProperty :public Property
{
public:
	IntProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Int; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class IntArrayProperty :public Property
{
public:
	IntArrayProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_IntArray; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class Vec2DArrayProperty :public Property
{
public:
	Vec2DArrayProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Vec2DArray; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class BoolProperty :public Property
{
public:
	BoolProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Bool; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class FloatProperty :public Property
{
public:
	FloatProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Float; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class RawStringProperty :public Property
{
public:
	RawStringProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_RawString; }
	IValue * GetValue(void *v) override;
};

class StdStringProperty :public Property
{
public:
	StdStringProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_StdString; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class Vec3DProperty :public Property
{
public:
	Vec3DProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Vec3D; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};
	
class RotatorProperty :public Property
{
public:
	RotatorProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Rotator; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class Vec2DProperty :public Property
{
public:
	Vec2DProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Vec2D; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class BoxProperty :public Property
{
public:
	BoxProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Bounds; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class PlaneProperty :public Property
{
public:
	PlaneProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Plane; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};

class ColorProperty :public Property
{
public:
	ColorProperty(int InOffset, EChannelMask InMask);
	EVarType GetValueType() override { return kV_Color; }
	IValue * GetValue(void *v) override;
	void OnSetValue(void *v, const IValue *value) override;
};




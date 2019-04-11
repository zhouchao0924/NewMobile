
#pragma once

#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kBox.h"
#include "Math/kPlane.h"
#include "Math/kColor.h"
#include <string>
#include <vector>
#include <assert.h>

enum EVarType
{
	kV_Nil,
	kV_Float,
	kV_Int,
	kV_Bool,
	kV_Color,
	kV_RawString,
	kV_StdString,
	kV_Vec4D,
	kV_Vec3D,
	kV_Vec2D,
	kV_Rotator,
	kV_Bounds,
	kV_IntArray,
	kV_Vec2DArray,
	kV_Plane,
};

template<class T> 
class kArray
{
public:
	kArray()
		:Elem(nullptr)
		,Num(0)
	{
	}

	kArray(std::vector<T> &Elms)
	{
		if (Elms.empty())
		{
			Elem = nullptr;
			Num = 0;
		}
		else
		{
			Elem = &Elms[0];
			Num = (int)Elms.size();
		}
	}

	T & operator[](int index)
	{
		assert(index < Num);
		return Elem[index];
	}

	T * data() { return Elem; }

	int size() const { return Num; }

	int bytes() const { return Num * sizeof(T); }

	void saveto(std::vector<T> &oth) const
	{
		oth.resize(Num);
		if (Num > 0)
		{
			memcpy(&oth[0], Elem, bytes());
		}
	}

private:
	T	*Elem;
	int  Num;
};

class IValue
{
public:
	virtual ~IValue() {}
	virtual void Retain() = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual bool IsNil() = 0;
	virtual EVarType GetType() const = 0;

	virtual int IntValue() const = 0;
	virtual float FloatValue() const = 0;
	virtual bool BoolValue() const = 0;
	virtual kPoint Vec2Value() const = 0;
	virtual kVector3D Vec3Value() const = 0;
	virtual kVector4D Vec4Value() const = 0;
	virtual kRotation RotationValue() const = 0;
	virtual kBox3D	Bounds() const = 0;
	virtual kPlane3D PlaneValue() const = 0;
	virtual kColor ColorValue() const = 0;
	virtual kArray<int> IntArrayValue() const = 0;
	virtual kArray<kPoint> Vec2ArrayValue() const = 0;
	virtual const char *StrValue() const = 0;
	virtual int GetNumFields() = 0;
	virtual IValue &GetField(int Index) = 0;
	virtual const char *GetFieldName(int Index) = 0;
	virtual IValue &GetField(const char *name) = 0;
	virtual bool AddField(const char *name, IValue &Value) = 0;
	virtual bool AddField(IValue &Value) = 0;
};

class IValueFactory
{
public:
	virtual ~IValueFactory() {}
	virtual IValue & Create() = 0;
	virtual IValue & Create(int Value) = 0;
	virtual IValue & Create(float Value) = 0;
	virtual IValue & Create(bool Value) = 0;
	virtual IValue & Create(kColor Value) = 0;
	virtual IValue & Create(const char *Value, bool bLight = true) = 0;
	virtual IValue & Create(std::string *Value, bool bLight = true) = 0;
	virtual IValue & Create(kVector3D *Value, bool bLight = true) = 0;
	virtual IValue & Create(kVector4D *Value, bool bLight = true) = 0;
	virtual IValue & Create(kRotation *Value, bool bLight = true) = 0;
	virtual IValue & Create(kPoint *Value, bool bLight = true) = 0;
	virtual IValue & Create(kBox3D *Value, bool bLight = true) = 0;
	virtual IValue & Create(const kArray<int> &Value, bool bLight = true) = 0;
	virtual IValue & Create(const kArray<kPoint> &Value, bool bLight = true) = 0;
	virtual IValue & Create(kPlane3D *Value, bool bLight = true) = 0;
	virtual void AutoRelease() = 0;
	virtual void DestroyValue(IValue *Value) = 0;
};

class IProperty 
{
public:
	virtual ~IProperty() {}
	virtual EVarType GetValueType() = 0;
	virtual IValue *GetValue(void *v) = 0;
	virtual void SetValue(void *v, const IValue *value) = 0;
	virtual const char *GetName() = 0;
	virtual void SetName(const char *InName) = 0;
	virtual bool IsReadOnly() = 0;
	virtual class IObjectDesc *GetDesc() = 0;
	virtual int GetOffset() = 0;
};

extern IValueFactory *GValueFactory;
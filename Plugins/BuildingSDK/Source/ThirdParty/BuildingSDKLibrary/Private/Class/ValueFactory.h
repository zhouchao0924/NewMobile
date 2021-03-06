
#pragma once

#include "PValue.h"

#define NEW_VALUE(value) {						\
		PValue  *pValue = new PValue(value);	\
		_AutoReleasePool.push_back(pValue);		\
		if(!bLight){							\
			pValue->Retain();					\
		}										\
		return *pValue;							\
}

#define NEW_LIGHT_VALUE(value) {				\
		PValue  *pValue = new PValue(value);	\
		_AutoReleasePool.push_back(pValue);		\
		return *pValue;							\
}


class ValueFactory :public IValueFactory
{
public:
	IValue & Create();
	IValue & Create(int Value) override { NEW_LIGHT_VALUE(Value) }
	IValue & Create(float Value) override { NEW_LIGHT_VALUE(Value) }
	IValue & Create(bool Value) override { NEW_LIGHT_VALUE(Value) }
	IValue & Create(kColor Value) override { NEW_LIGHT_VALUE(Value) }
	IValue & Create(const char *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(std::string *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(kVector3D *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(kVector4D *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(kRotation *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(kPoint *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(kBox3D *Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(const kArray<int>  &Value, bool bLight = true) override { NEW_VALUE(Value) }
	IValue & Create(const kArray<kPoint> &Value, bool bLight = true) { NEW_VALUE(Value) }
	IValue & Create(kPlane3D *Value, bool bLight = true) override { NEW_VALUE(Value) }
	void AutoRelease() override;
	void DestroyValue(IValue *Value) override;
protected:
	std::vector<PValue *> _AutoReleasePool;
};



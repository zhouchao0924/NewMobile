
#include "IProperty.h"
#include "BuildingObject.h"
#include "Class/Property.h"
#include "Class/PValue.h"
#include "SuiteImpl.h"

BuildingObject::BuildingObject()
	:_Suite(nullptr)
{
}

BEGIN_DERIVED_CLASS(BuildingObject, Object)
	ADD_PROP_READONLY(AnchorSlots, IntArrayProperty)
END_CLASS()

void BuildingObject::Serialize(ISerialize &Ar, unsigned int Ver)
{
	Object::Serialize(Ar, Ver);

	BeginChunk<BuildingObject>(Ar);
	
	EndChunk<BuildingObject>(Ar);
}

void BuildingObject::MarkNeedUpdate(EChannelMask Mask /*= EChannelAll*/)
{
	SuiteImpl *Impl = (SuiteImpl *)_Suite;
	if (Impl)
	{
		Impl->DeferredUpdate(this, Mask);
	}
}

void BuildingObject::Delete()
{
	if (_Suite)
	{
		_Suite->DeleteObject(_ID, true);
	}
}



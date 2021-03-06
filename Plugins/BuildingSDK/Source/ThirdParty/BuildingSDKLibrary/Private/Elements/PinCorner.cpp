

#include "ISuite.h"
#include "IClass.h"
#include "PinCorner.h"
#include "BuildingObject.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(PinCorner, Corner)
	ADD_PROP(OwnerID, IntProperty)
END_CLASS()

PinCorner::PinCorner()
	:OwnerID(INVALID_OBJID)
{
}

void PinCorner::Update()
{
	BuildingObject *pOwner = SUITE_GET_BUILDING_OBJ(OwnerID, BuildingObject);
	
	if (pOwner)
	{
		pOwner->MarkNeedUpdate();
	}

	Corner::Update();
}

bool PinCorner::IsFree()
{
	return Corner::IsFree() && OwnerID == INVALID_OBJID;
}







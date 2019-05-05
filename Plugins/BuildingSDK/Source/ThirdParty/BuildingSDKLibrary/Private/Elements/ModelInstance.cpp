
#include "ISuite.h"
#include "Anchor.h"
#include "Class/Property.h"
#include "ModelInstance.h"
#include "SuiteImpl.h"

BEGIN_DERIVED_CLASS(ModelInstance, BuildingObject)
	ADD_PROP(ResID, StdStringProperty)
	ADD_PROP(Type, IntProperty)
	ADD_PROP(AnchorID, IntProperty)
	ADD_PROP_CHANNEL(Location, Vec3DProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(Size, Vec3DProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(Forward, Vec3DProperty, EChannelGeometry)
	ADD_PROP_CHANNEL(AlignType, IntProperty, EChannelGeometry)
	ADD_PROP(ClipBase0, Vec3DProperty)
	ADD_PROP(ClipBase1, Vec3DProperty)
	ADD_PROP(ClipNormal0, Vec3DProperty)
	ADD_PROP(ClipNormal1, Vec3DProperty)
END_CLASS()

ModelInstance::ModelInstance()
	: AnchorID(INVALID_OBJID)
	, Type(-1)
{
}

Anchor *ModelInstance::CreateAnchor()
{
	Anchor *pAnchor = nullptr;
	if (_Suite)
	{
		if (AnchorSlots.empty())
		{
			SuiteImpl *impl = (SuiteImpl *)_Suite;
			pAnchor = (Anchor *)impl->CreateObject(EAnchor);
			pAnchor->OwnerID = _ID;
			impl->OnCreate(pAnchor);
			AnchorSlots.push_back(pAnchor->GetID());
		}
		else
		{
			pAnchor = SUITE_GET_BUILDING_OBJ(AnchorSlots[0], Anchor);
		}
	}
	return pAnchor;
}

ModelInstance *ModelInstance::GetModelByType(int type)
{
	if (!AnchorSlots.empty())
	{
		Anchor *pAnchor = SUITE_GET_BUILDING_OBJ(AnchorSlots[0], Anchor);
		if (pAnchor)
		{
			return pAnchor->GetModelByType(type);
		}
	}
	return nullptr;
}

ModelInstance * ModelInstance::SetModelByType(int type, const std::string ResID)
{
	if (!AnchorSlots.empty())
	{
		Anchor *pAnchor = SUITE_GET_BUILDING_OBJ(AnchorSlots[0], Anchor);
		if (pAnchor)
		{
			return pAnchor->SetModelByType(type, ResID);
		}
	}
	return nullptr;
}

IValue *ModelInstance::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = BuildingObject::GetFunctionProperty(name);
	if (!pValue)
	{
		if (name == "Parent")
		{
			Anchor *pAnchor = SUITE_GET_BUILDING_OBJ(AnchorID, Anchor);
			if (pAnchor)
			{
				pValue = &GValueFactory->Create(pAnchor->OwnerID);
			}
		}
	}
	return pValue;
}

void ModelInstance::Serialize(ISerialize &Ar, unsigned int Ver)
{
	BuildingObject::Serialize(Ar, Ver);

	BeginChunk<ModelInstance>(Ar);
	
	EndChunk<ModelInstance>(Ar);
}

bool ModelInstance::IsRoot()
{
	bool bRoot = false;
	Anchor *pAnchor = SUITE_GET_BUILDING_OBJ(AnchorID, Anchor);
	if (!pAnchor || pAnchor->IsA(EWallHole))
	{
		bRoot = true;
	}
	return bRoot;
}

void ModelInstance::OnDestroy()
{
	Anchor *pAnchor = SUITE_GET_BUILDING_OBJ(AnchorID, Anchor);
	if (pAnchor)
	{
		pAnchor->UnLink(this);
	}
	
	BuildingObject::OnDestroy();
}


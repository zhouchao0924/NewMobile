
#include "ISuite.h"
#include "Anchor.h"
#include "Corner.h"
#include "ModelInstance.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(Anchor, BuildingObject)
	ADD_PROP_READONLY(OwnerID, IntProperty)
	ADD_PROP_READONLY(EditPoints, IntArrayProperty)
	ADD_PROP_READONLY(LinkObjects, IntArrayProperty)
END_CLASS()

Anchor::Anchor()
	:OwnerID(INVALID_OBJID)
{
}

void Anchor::Serialize(ISerialize &Ar, unsigned int Ver)
{
	BuildingObject::Serialize(Ar, Ver);
}

IValue *Anchor::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = BuildingObject::GetFunctionProperty(name);

	if (!pValue)
	{
		if (name == "Location")
		{
			kPoint Location = GetCornerLocation();
			pValue = &GValueFactory->Create(&Location, false);
		}
		else if (name == "CornerID")
		{
			ObjectID ID = INVALID_OBJID;
			if (!EditPoints.empty())
			{
				ID = EditPoints[0];
			}
			pValue = &GValueFactory->Create(ID);
		}
	}

	return pValue;
}

bool Anchor::SetFunctionProperty(const std::string &name, const IValue *Value)
{
	if (Value && name == "Location")
	{
		Corner *pCorner = SUITE_GET_BUILDING_OBJ(GetCorner(0), Corner);
		if (pCorner)
		{
			pCorner->SetLocation(Value->Vec2Value());
		}
		return true;
	}
	return false;
}

kPoint Anchor::GetCornerLocation()
{
	Corner *pCorner = SUITE_GET_BUILDING_OBJ(GetCorner(0), Corner);
	if (pCorner)
	{
		return pCorner->Location;
	}
	return kPoint();
}

bool Anchor::GetLocation(kVector3D &Location)
{
	Corner *pCorner = SUITE_GET_BUILDING_OBJ(GetCorner(0), Corner);
	if (pCorner)
	{
		Location = pCorner->Location;
		return true;
	}
	return false;
}

void Anchor::Link(ModelInstance *InModel)
{
	if (InModel)
	{
		ObjectID ModelID = InModel->GetID();
			
		size_t i = 0;
		for (; i < LinkObjects.size(); ++i);

		if (i >= LinkObjects.size())
		{
			InModel->AnchorID = GetID();
			UpdateTransform(InModel);
			LinkObjects.push_back(ModelID);
		}
	}
}

void Anchor::Update() 
{
	for (size_t i = 0; i < LinkObjects.size(); ++i)
	{
		ModelInstance *pModel =  SUITE_GET_BUILDING_OBJ(LinkObjects[i], ModelInstance);
		if (pModel)
		{
			UpdateTransform(pModel);
			pModel->MarkNeedUpdate();
		}
	}
}

void Anchor::UpdateTransform(ModelInstance *pModel)
{
	kVector3D Location;
	if (GetLocation(Location))
	{
		pModel->Location = Location;
	}
}

void Anchor::UnLink(ModelInstance *InModel)
{
	if (InModel)
	{
		ObjectID ModelID = InModel->GetID();

		size_t i = 0;
		for (; i < LinkObjects.size(); ++i);
		
		if (i < LinkObjects.size())
		{
			LinkObjects.erase(LinkObjects.begin() + i);
		}
	}
}

ObjectID Anchor::GetModel(int index /*= 0*/)
{
	if (LinkObjects.empty())
	{
		return INVALID_OBJID;
	}
	return LinkObjects[0];
}

ModelInstance *Anchor::GetModelByType(int type)
{
	for (int i = 0; i < LinkObjects.size(); ++i)
	{
		ModelInstance *pModel = SUITE_GET_BUILDING_OBJ(LinkObjects[i], ModelInstance);
		if (pModel && pModel->Type == type)
		{
			return pModel;
		}
	}
	return nullptr;
}

ModelInstance * Anchor::SetModelByType(int type, const std::string ResID)
{
	ModelInstance *pModel = GetModelByType(0);
	if (!pModel)
	{
		ObjectID ModelID = _Suite->AddModelToAnchor(GetID(), ResID.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 0);
		pModel = SUITE_GET_BUILDING_OBJ(ModelID, ModelInstance);
	}

	if (pModel)
	{
		pModel->ResID = ResID;
		pModel->MarkNeedUpdate();
	}

	return pModel;
}

ObjectID Anchor::GetCorner(int index)
{
	ObjectID ID = INVALID_OBJID;
	if (!EditPoints.empty())
	{
		ID = EditPoints[0];
	}
	return ID;
}

void Anchor::SetCorner(int index, ObjectID InCornerID)
{
	if (index >= (int)EditPoints.size())
	{
		EditPoints.resize(index + 1);
	}
	EditPoints[index] = InCornerID;
}

void Anchor::OnDestroy()
{
	if (_Suite)
	{
		for (size_t i = 0; i < EditPoints.size(); ++i)
		{
			ObjectID ID = EditPoints[i];
			_Suite->DeleteObject(ID, true);
		}
			
		for (size_t i = 0; i < LinkObjects.size(); ++i)
		{
			ObjectID ObjID = LinkObjects[i];
			_Suite->DeleteObject(ObjID, true);
		}

		EditPoints.size();
		LinkObjects.clear();
	}

	BuildingObject::OnDestroy();
}



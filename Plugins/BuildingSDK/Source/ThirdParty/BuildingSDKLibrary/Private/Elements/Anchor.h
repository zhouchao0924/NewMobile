
#pragma once

#include "BuildingObject.h"

class ModelInstance;
class Anchor :public BuildingObject
{
	DEFIN_CLASS()
public:
	Anchor();
	void Serialize(ISerialize &Ar, unsigned int Ver);
	void OnDestroy();
	EObjectType GetType() { return EAnchor; }
	virtual void Link(ModelInstance *InModel);
	virtual void UnLink(ModelInstance *InModel);
	virtual ObjectID GetModel(int index = 0);
	virtual void UpdateTransform(ModelInstance *pModel);
	bool SetFunctionProperty(const std::string &name, const IValue *Value) override;
	IValue *GetFunctionProperty(const std::string &name) override;
	ModelInstance *GetModelByType(int type);
	ModelInstance *SetModelByType(int type, const std::string ResID);
	void Update() override;
	virtual bool GetLocation(kVector3D &Location);
	kPoint GetCornerLocation();
	void SetCorner(int index, ObjectID InCornerID);
	ObjectID GetCorner(int index);
public:
	ObjectID				OwnerID;
	std::vector<ObjectID>	EditPoints;
	std::vector<ObjectID>	LinkObjects;
};




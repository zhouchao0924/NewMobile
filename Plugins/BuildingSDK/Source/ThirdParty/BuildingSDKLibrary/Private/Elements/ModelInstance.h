
#pragma once

#include "kBox.h"
#include "kPlane.h"
#include "BuildingObject.h"

enum EModelAlignType
{
	EAlignOriginal,

	EAlignBottomCenter,
	EAlignTopCenter,
	EAlignLeftCenter,
	EAlignRightCenter,
	EAlignFrontCenter,
	EAlignBackCenter,

	EAlignTopFrontEdgeCenter,
	EAlignTopBackEdgeCenter,
	EAlignTopLeftEdgeCenter,
	EAlignTopRightEdgeCenter,

	EAlignBottomFrontEdgeCenter,
	EAlignBottomBackEdgeCenter,
	EAlignBottomLeftEdgeCenter,
	EAlignBottomRightEdgeCenter,

	EAlignLeftFrontEdgeCenter,
	EAlignRightFrontEdgeCenter,
	EAlignLeftBackEdgeCenter,
	EAlignRightBackEdgeCenter
};

class ModelInstance :public BuildingObject
{
	DEFIN_CLASS()
public:
	ModelInstance();
	bool IsRoot() override;
	EObjectType GetType() { return EModelInstance; }
	void OnDestroy() override;
	void Serialize(ISerialize &Ar, unsigned int Ver);
	Anchor *CreateAnchor() override;
	ModelInstance *GetModelByType(int type);
	ModelInstance *SetModelByType(int type, const std::string ResID);
	IValue *GetFunctionProperty(const std::string &name) override;
public:	
	int					Type;
	ObjectID			AnchorID;
	std::string			ResID;
	kVector3D			Location;
	kVector3D			Size;
	kVector3D			Forward;
	EModelAlignType		AlignType;
	kVector3D			ClipBase0;
	kVector3D			ClipBase1;
	kVector3D			ClipNormal0;
	kVector3D			ClipNormal1;
};





#include "ISuite.h"
#include "Math/kBox.h"
#include "Elements/Wall.h"
#include "Elements/Window.h"
#include "Elements/DoorHole.h"
#include "Elements/Room.h"
#include "Elements/Anchor.h"
#include "Elements/Surface.h"
#include "Elements/Skirting.h"
#include "Elements/BuildingConfig.h"
#include "Elements/ModelInstance.h"
#include "Elements/PlanePrimitive.h"
#include "Elements/Corner.h"
#include "Elements/CompoudObject.h"
#include "Elements/PinCorner.h"
#include "Elements/Light.h"
#include "Elements/SPotLight.h"
#include "Elements/PointLight.h"
#include "Elements/PostProcess.h"
#include "Elements/SkyLight.h"
#include "MXFile/MXFile.h"
#include "ClassLibaray.h"

#define REG_CLASS(classname){							\
		classname::BuildProperties();					\
		RegisterClass(classname::GetObjectDesc());		\
}

void FClassLibaray::InitClassLibaray()
{
	REG_CLASS(BuildingObject)
	REG_CLASS(Corner)
	REG_CLASS(PinCorner)
	REG_CLASS(PlanePrimitive)
	REG_CLASS(FloorPlane)
	REG_CLASS(CeilPlane)
	REG_CLASS(Room)
	REG_CLASS(Wall)
	REG_CLASS(SolidWall)
	REG_CLASS(VirtualWall)
	REG_CLASS(WallHole)
	REG_CLASS(Window)
	REG_CLASS(DoorHole)
	REG_CLASS(Anchor)
	REG_CLASS(ModelInstance)
	REG_CLASS(Surface)
	REG_CLASS(BuildingConfig)
	REG_CLASS(Skirting)
	REG_CLASS(MXFile)
	REG_CLASS(CompoundObject)
	REG_CLASS(Light)
	REG_CLASS(PointLight)
	REG_CLASS(SpotLight)
	REG_CLASS(SkyLight)
	REG_CLASS(PostProcess)
}

void FClassLibaray::RegisterClass(ObjectDesc *ObjectClass)
{
	if (_ClassMap.find(ObjectClass->ObjectType) == _ClassMap.end())
	{
		_ClassMap[ObjectClass->ObjectType] = ObjectClass;
	}
}

Object *FClassLibaray::CreateObject(int ObjectType)
{
	Object *NewObj = nullptr;

	switch (ObjectType)
	{
		case ECorner: NewObj = new Corner(); break;
		case EPinCorner: NewObj = new PinCorner(); break;
		case EPlane: NewObj = new PlanePrimitive(); break;
		case EModelInstance: NewObj = new ModelInstance(); break;
		case ERoom: NewObj = new Room(); break;
		case ESkirting: NewObj = new Skirting(); break;
		case ECeilPlane: NewObj = new CeilPlane(); break;
		case EFloorPlane: NewObj = new FloorPlane(); break; 
		case EWindow: NewObj = new Window(); break;
		case EDoorHole: NewObj = new DoorHole(); break;
		case EAnchor: NewObj = new Anchor(); break;
		case ESurface: NewObj = new Surface(); break;
		case EBuildingConfig: NewObj = new BuildingConfig(); break;
		case EMXFile: NewObj = new MXFile(); break;
		case ESolidWall: NewObj = new SolidWall(); break;
		case EVirtualWall: NewObj = new VirtualWall(); break;
		case ECompoundObject: NewObj = new CompoundObject(); break;
		case ELight: NewObj = new Light(); break;
		case EPointLight: NewObj = new PointLight(); break;
		case ESpotLight: NewObj = new SpotLight(); break;
		case ESkyLight: NewObj = new SkyLight(); break;
		case EPostProcess: NewObj = new PostProcess(); break;
		default: break;
	}

	if (NewObj)
	{
		ClassMap::iterator it = _ClassMap.find(ObjectType);
		if (it != _ClassMap.end())
		{
			NewObj->_ClsDesc = it->second;
		}
	}

	return NewObj;
}



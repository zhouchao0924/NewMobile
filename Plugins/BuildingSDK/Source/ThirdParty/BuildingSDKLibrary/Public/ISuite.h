
#pragma once

#include <vector>
#include "Math/kLine.h"
#include "Math/kVector2D.h"
#include "Math/kBox.h"
#include "ISerialize.h"
#include "IProperty.h"
#include "IClass.h"

class ISuiteListener
{
public:
	virtual void OnAddObject(IObject *RawObj) = 0;
	virtual void OnDeleteObject(IObject *RawObj) = 0;
	virtual void OnUpdateObject(IObject *RawObj, unsigned int ChannelMask) = 0;
	virtual void OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID Surface) = 0;
};

enum kESnapType
{
	kEPt,
	kEDirX,
	kEDirY,
	kENone
};

class ISuite
{
public:
	virtual ~ISuite() { } 
	virtual ObjectID GetConfig() = 0;

	virtual ObjectID AddCorner(float x, float y) = 0;
	virtual ObjectID AddModelToObject(ObjectID BaseObjID, const char *ResID, const kVector3D &Location, const kRotation &Rotation = kRotation(), const kVector3D &Size = kVector3D(0.0f), int Type = -1) = 0;
	virtual ObjectID AddModelToAnchor(ObjectID AnchorID, const char *ResID, const kVector3D &Location, const kRotation &Rotation = kRotation(), const kVector3D &Size = kVector3D(0.0f), int Type = -1) = 0;

	virtual void AddWall(ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height) = 0;
	virtual void AddVirtualWall(ObjectID StartCorner, ObjectID EndCorner) = 0;
	virtual ObjectID AddWindow(ObjectID WallID, const kPoint &Location, float zPos, float Width, float Height) = 0;
	virtual ObjectID AddDoor(ObjectID WallID, const kPoint &Location, float Width, float Height, float zPos = 0) = 0;
	virtual ObjectID AddPointLight(const kVector3D &Location, float SourceRadius, float SoftSourceRadius, float SourceLength, float Intensity, kColor LightColor, bool bCastShadow) = 0;
	virtual ObjectID AddSpotLight(const kVector3D &Location, const kRotation &Rotationn, float AttenuationRadius,  float SourceRadius, float SoftSourceRadius, float SourceLength, float InnerConeAngle, float OuterConeAngle, float Intensity, kColor LightColor, bool bCastShadow) = 0;

	virtual ObjectID GetConnectCorner(ObjectID Wall0, ObjectID Wall1, bool &bWall0InverseConnect) = 0;
	virtual ObjectID FindCloseWall(const kPoint &Location, float Width, kPoint &BestLoc, float Tolerance = -1.0f) = 0;	
	virtual IObject *GetObject(ObjectID ObjID, EObjectType InClass = EUnkownObject) = 0;
	virtual int  GetAllObjects(IObject **&ppObjects, EObjectType InClass = EUnkownObject, bool bIncludeDeriveType = true) = 0;
	virtual bool Move(ObjectID ObjID, const kPoint &DeltaMove) = 0;
	virtual void DeleteObject(ObjectID ObjID, bool bForce = false) = 0;
	virtual bool IsFree(ObjectID CornerID) = 0;
	virtual ObjectID GetWallByTwoCorner(ObjectID CornerID0, ObjectID CornerID1) = 0;
	virtual int  GetConnectWalls(ObjectID CorerID, ObjectID *&pConnectedWalls) = 0;		
	virtual bool GetWallForwardFromCorner(ObjectID WallID, ObjectID CornerID, kPoint &Forward) = 0;
	virtual bool GetWallVector(ObjectID WallID, kPoint &P0, kPoint &P1, kPoint &Right) = 0;
	virtual bool GetWallBorderLines(ObjectID WallID, kLine &CenterLine, kLine &LeftLine, kLine &RightLine) = 0;		
	virtual int  GetPolygon(ObjectID RoomID, kPoint *&pPolygons, bool bInner) = 0;
	virtual void GetRoomCorners(ObjectID RoomID, std::vector<ObjectID> &Corners) = 0;
	virtual void SetListener(ISuiteListener *Listener) = 0;
	virtual void Serialize(ISerialize &Ar) = 0;
	virtual bool Load(const char *Filename) = 0;
	virtual void Save(const char *Filename) = 0;
	virtual kESnapType FindSnapLocation(const kPoint &TouchPos, kPoint &BestSnapLocation, ObjectID &BestObjID, ObjectID IgnoreObjID, float Tolerance = -1.0f) = 0;
	virtual void Update() = 0;
	virtual void BuildRooms() = 0;
	virtual bool IsRoomCeilVisible() = 0;
	virtual void SetRoomCeilVisible(bool bVisible) = 0;
	virtual IMeshObject *GetMeshObject(ObjectID ID) = 0;
	virtual ObjectID SetSurface(ObjectID PrimitiveID, const char *MaterialName, int MaterialType, int SubModelIndex = -1) = 0;
	virtual void SetSurface(ObjectID PrimitiveID, ObjectID SurfaceID, int SubModelIndex = -1) = 0;
	virtual ObjectID GetSurface(ObjectID PrimitiveID, int SubModelIndex) = 0;
	virtual void RemoveSurface(ObjectID PrimitiveID, int SubModelIndex) = 0;
	virtual ObjectID HitTest(const kPoint &Location) = 0;
	virtual ObjectID GetRoomByLocation(const kPoint &Location) = 0;
	virtual int HitTest(const kPoint &Min, const kPoint &Max, ObjectID *&pResults) = 0;
	virtual IValue &GetProperty(ObjectID ID, const char *PropertyName) = 0;
	virtual void SetProperty(ObjectID ID, const char *PropertyName, const IValue *Value) = 0;
	virtual void SetValue(ObjectID ID, const char *PropertyName, IValue *Value) = 0;
	virtual IValue *FindValue(ObjectID ID, const char *PropertyName) = 0;
};

#define SUITE_GET_BUILDING_OBJ(ID, classtype) (classtype *)_Suite->GetObject(ID, E##classtype);
#define GET_BUILDING_OBJ(suite,ID, classtype) (classtype *)suite->GetObject(ID, E##classtype);

typedef void * (*LoadSDKFunctionType)();
#define  Export_SDKFunaction  ("LoadSDKFunction")

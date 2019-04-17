
#pragma once

#include "IClass.h"
#include "ISerialize.h"
#include "IProperty.h"

enum EObjectType
{
	EUnkownObject,
	EObject,
	ECompoundObject,
	EBuildingObject,
	EBuildingConfig,
	EPrimitive,
	ECorner,
	EPinCorner,
	EPlane,
	EModelInstance,
	EWallOpening,
	EWall,
	EVirtualWall,
	ESolidWall,
	EWallHole,
	EWindow,
	EDoorHole,
	EArcWall,
	EBezierWall,
	ESpecialWall,
	ESkirting,
	ERoom,
	EPlanePrimitive,
	EFloorPlane,
	ECeilPlane,
	EPillar,
	EPlatform,
	ESurface,
	EAnchor,
	EMXFile,
	ELight,
	EPointLight,
	ESpotLight,
	ESkyLight,
	EPostProcess,
};

#define  INVALID_OBJID -1
typedef	 int  ObjectID;
class IMeshObject;

class IObjectDesc
{
public:
	virtual const char *GetObjectClassName() = 0;
	virtual void AddProperty(const char *InName, IProperty *Prop, bool bReadOnly) = 0;
	virtual int GetNumberOfProperty() = 0;
	virtual class IProperty *GetProperty(int index) = 0;
	virtual class IProperty *GetProperty(const char *Propname) = 0;
	virtual IObjectDesc *GetSuperDesc() = 0;
};

class IAttributable
{
public:
	virtual int GetNumberOfProperties() = 0;
	virtual IProperty *GetProperty(int Index) = 0;
	virtual IProperty *GetProperty(const char  *Name) = 0;
	virtual IValue &GetPropertyValue(const char  *Name) = 0;
	virtual void SetPropertyValue(const char *Name, const IValue *Value) = 0;
};

enum ETexSlot
{
	eDiffuse,
	eSpecular,
	eEmissive,
	eRoughness,
	eMetallic,
	eOpacity,
	eNormal,
	eMax
};

enum ETextureFormat
{
	Tex_A8R8G8B8 = 0,
	Tex_DXT5 = 6,
	Tex_A8 = 7,
};

enum EChannelMask
{
	EChannelNone = 0x0,
	EChannelGeometry = 0x1,
	EChannelSurface = 0x2,
	EChannelTransform = 0x4,
	EChannelUserData = 0x8,
	EChannelAll = 0xffffffff,
};

class IObject :public IAttributable
{
public:
	virtual ~IObject(){ }
	virtual ObjectID GetID() = 0;
	virtual class ISuite *GetSuite() = 0;
	virtual bool IsA(EObjectType Type) = 0;
	virtual bool IsRoot() = 0;
	virtual void Serialize(ISerialize &Ar, unsigned int Ver) = 0;
	virtual unsigned int GetVersion() = 0;
	virtual bool IsDeletable() = 0;
	virtual int  GetMeshCount() = 0;
	virtual class IMeshObject *GetMeshObject(int MeshIndex = 0, bool bBuildIfNotExist = true) = 0;
	virtual int  GetSurfaceCount() = 0;
	virtual class ISurfaceObject *GetSurfaceObject(int SurfaceIndex) = 0;
	virtual bool  HitTest(const kVector3D &RayStart, const kVector3D &RayDir, ObjectID &OutSurfaceID, int *OutSectionIndex = nullptr, kVector3D *OutPosition = nullptr, kVector3D *OutNormal = nullptr) = 0;
	virtual int   GetTextureCount() = 0;
	virtual bool  GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, int &Format) = 0;
	virtual bool  GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes) = 0;
	virtual bool  SaveTextureToFile(int iTex, const char *Filename) = 0;
	virtual EObjectType GetType() = 0;
	virtual IObjectDesc *GetDesc() = 0;
	virtual bool Move(const kPoint &DeltaMove) = 0;
	virtual IValue *GetValueArray() = 0;
	virtual IValue *FindValue(const char *name) = 0;
	virtual void SetValue(const char *name, IValue *value) = 0;
	virtual void MarkNeedUpdate(EChannelMask Mask = EChannelAll) = 0;
	virtual void Delete() = 0;
};



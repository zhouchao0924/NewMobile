
#pragma once

#include "IClass.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kColor.h"

class IMeshObjectCallback
{
public:
	virtual ISurfaceObject *GetMeshSurface(int SurfaceID) = 0;
	virtual void Link(int SurfaceID, int SubModelIndex) = 0;
	virtual void UnLink(int SurfaceID, int SubModelIndex) = 0;
};

class IMeshObject
{
public:
	virtual int  GetSectionCount() = 0;
	virtual bool GetSectionMesh(int SectionIndex, float *&pVertices, float *&pNormals, float *&pTangents, float *&pUVs, float *&pLightmapUVs, int &NumVerts, int *&pIndices, int &NumIndices) = 0;
	virtual int  GetSectionSurfaceID(int SubSectionIndex) = 0;
	virtual class ISurfaceObject *GetSectionSurface(int SubSectionIndex) = 0;
	virtual void SetSectionSurface(int SubSectionIndex, ObjectID SurfaceID) = 0;
	virtual void SetMeshObjectCallback(IMeshObjectCallback *Callback) = 0;
	virtual void AddQuad(struct FMeshSection *Mesh, const kVector3D &V0, const kVector3D &V1, const kVector3D &V2, const kVector3D &V3, const kVector3D &Normal, const kVector3D &Tan, int MaterialIndex) = 0;
	virtual int  AddVert(struct FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightmapUV) = 0;
	virtual int  AddVertDefaultUV(struct FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &LightmapUV) = 0;
	virtual void AddTri(struct FMeshSection *Mesh, int V0, int V1, int V2) = 0;
	virtual bool HitTest(const kVector3D &RayStart, const kVector3D &RayDir, int *OutSectionIndex = nullptr, kVector3D *OutPosition = nullptr, kVector3D *OutNormal = nullptr) = 0;
};

enum ESurfaceType
{
	ERefMaterial,
	EUE4Material,
};

class ISurfaceType
{
public:
	virtual int GetType() = 0;
};

class ISurfaceObject
{
public:
	virtual int GetType() = 0;
	virtual const char *GetUri() = 0;
	virtual void SetSource(int type, const char *Uri) = 0;

	virtual void SetScalar(const char *name, float Value) = 0;
	virtual void SetVector2D(const char *name, const kPoint &Value) = 0;
	virtual void SetVector3D(const char *name, const kVector3D &Value) = 0;
	virtual void SetVector(const char *name, const kVector4D &Value) = 0;

	virtual float GetScalar(const char *name) = 0;
	virtual kPoint GetVector2D(const char *name) = 0;
	virtual kVector3D GetVector3D(const char *name) = 0;
	virtual kVector4D GetVector(const char *name) = 0;

	virtual int GetTexture(ETexSlot slot) = 0;
	virtual IValue *FindParamValue(const char *name) = 0;
};



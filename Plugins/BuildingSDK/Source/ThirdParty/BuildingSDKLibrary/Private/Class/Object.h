
#pragma once

#include "IProperty.h"
#include "ISerialize.h"
#include "IClass.h"
#include "ObjectDesc.h"

class Object : public IObject
{
	DEFIN_CLASS()
public:
	Object();
	~Object();
	ObjectID GetID() override { return _ID; }
	EObjectType GetType() override { return EObject; }
	bool IsA(EObjectType Type) override;
	bool IsRoot() override { return false; }
	void Serialize(ISerialize &Ar, unsigned int Ver) override;
	unsigned int  GetVersion() override { return 0; }
	bool IsDeletable() override { return false; }
	bool Move(const kPoint &DeltaMove) { return false; }
	ISuite *GetSuite() { return _Suite; }
	int   GetMeshCount() override { return 0; }
	class IMeshObject *GetMeshObject(int MeshIndex, bool bBuildIfNotExist = true) override { return nullptr; }
	bool HitTest(const kVector3D &RayStart, const kVector3D &RayDir, ObjectID &OutSurfaceID, int *OutSectionIndex = nullptr, kVector3D *OutPosition = nullptr, kVector3D *OutNormal = nullptr) override;
	int  GetSurfaceCount() override { return 0; }
	class ISurfaceObject *GetSurfaceObject(int SurfaceIndex) override { return nullptr; }
	IObjectDesc *GetDesc() override { return _ClsDesc; }
	int GetNumberOfProperties() override;
	IProperty *GetProperty(int Index) override;
	IProperty *GetProperty(const char  *Name) override;
	IValue &GetPropertyValue(const char  *Name) override;
	void SetPropertyValue(const char *Name, const IValue *Value);
	virtual IValue *GetFunctionProperty(const std::string &name) { return nullptr; }
	virtual bool SetFunctionProperty(const std::string &name, const IValue *Value) { return false; }
	int   GetTextureCount() { return 0; }
	bool  GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, int &Format) { return false; }
	bool  GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes) { return false; }
	bool  SaveTextureToFile(int iTex, const char *Filename);
	IValue *FindValue(const char *name);
	void SetValue(const char *name, IValue *value);
	IValue *GetValueArray() override;
	virtual void Update() {}
	virtual void Delete();
	virtual bool IsNeedUpdate() { return _bNeedUpdate; }
	virtual void MarkNeedUpdate(EChannelMask Mask = EChannelAll) {}
protected:
	IValue *REG_GetPropertyValue(const char  *Name);
	bool REG_SetPropertyValue(const char *Name, const IValue *Value);
	void GetAllDesc(std::vector<ObjectDesc*> &Descs, std::vector<int> &DescCounts);
	void SerializeProperties(ISerialize &Ar);
	void SerializeDictionary(ISerialize &Ar);
	void GetProperties(std::vector<IProperty *> &Properties);
	ObjectDesc	*_ClsDesc;
protected:
	friend class		 FClassLibaray;
	ObjectID			 _ID;
	class ISuite		*_Suite;
 	bool				 _bNeedUpdate;
	std::unordered_map<std::string, IValue *> _dictionary;
};

template<class T>
inline void BeginChunk(ISerialize &Ar)
{
	int ChunkID = 0;
	ObjectDesc *Desc = T::GetObjectDesc();
	if (Desc)
	{
		if (Ar.IsLoading())
		{
			ChunkID = Ar.ReadChunk();
		}
		else if (Ar.IsSaving())
		{
			ChunkID = Desc->ObjectType;
			Ar.WriteChunk(ChunkID);
		}
	}
}

template<class T>
inline void EndChunk(ISerialize &Ar)
{
	ObjectDesc *Desc = T::GetObjectDesc();
	if (Desc)
	{
		Ar.EndChunk(Desc->ObjectType);
	}
}



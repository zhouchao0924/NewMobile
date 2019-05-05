
#pragma once

#include "Platform.h"
#include "Mesh/MeshObject.h"

#if RS_PLATFORM==PLATFORM_WINDOWS && !USE_MX_ONLY
#include "fbxsdk.h"

class FBXImporterImpl
{
public:
	FBXImporterImpl();
	static FBXImporterImpl *Get(bool bCreateIfNotExsit = true);
	class FBXFile *Import(const char *fbxFilename);
	void Initialize();
	void UnInitialize();
protected:
	bool InitFbxScene(const char *fbxFilename);
	void DestroyFbxScene();
	void ImportMesh(FbxMesh *Mesh);
	void ImportNaterial(FbxSurfaceMaterial *fbxMaterial);
	int  ImportTexture(ETexSlot slot, const std::string &texFilename);
	void GetFbxMeshUVSet(FbxMesh *fbxMesh, std::vector<std::string> &UVSets);
	void GetFbxMeshMaterials(FbxMesh *fbxMesh);
protected:
	FbxManager					*m_pFbxManger;
	FbxScene					*m_pScene;
	FbxImporter					*m_Importer;
	std::vector<MeshObject *>	 m_MesheObjects;
};

#endif



#include "Platform.h"
#include "FBXImportImpl.h"

#if RS_PLATFORM==PLATFORM_WINDOWS &&  !USE_MX_ONLY

#if NDEBUG
	#pragma comment(lib, "Thridparty/FBX/lib/x64/release/libfbxsdk-md.lib")	
#else
	#pragma comment(lib, "Thridparty/FBX/lib/x64/debug/libfbxsdk-md.lib")
#endif

static FBXImporterImpl *GImporter = nullptr;

FBXImporterImpl::FBXImporterImpl()
	: m_pFbxManger(nullptr)
	, m_pScene(nullptr)
	, m_Importer(nullptr)
{
}

FBXImporterImpl *FBXImporterImpl::Get(bool bCreateIfNotExsit)
{
	if (bCreateIfNotExsit && GImporter)
	{
		GImporter = new FBXImporterImpl();
		GImporter->Initialize();
	}
	return GImporter;
}

void FBXImporterImpl::Initialize()
{
	m_pFbxManger = FbxManager::Create();

	if (m_pFbxManger)
	{
		FbxIOSettings* ios = FbxIOSettings::Create(m_pFbxManger, IOSROOT);
		m_pFbxManger->SetIOSettings(ios);

		ios->SetBoolProp(IMP_FBX_MATERIAL, true);
		ios->SetBoolProp(IMP_FBX_TEXTURE, true);
		ios->SetBoolProp(IMP_FBX_LINK, true);
		ios->SetBoolProp(IMP_FBX_SHAPE, true);
		ios->SetBoolProp(IMP_FBX_GOBO, true);
		ios->SetBoolProp(IMP_FBX_ANIMATION, true);
		ios->SetBoolProp(IMP_SKINS, true);
		ios->SetBoolProp(IMP_DEFORMATION, true);
		ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		ios->SetBoolProp(IMP_TAKE, true);

		int lSDKMajor, lSDKMinor, lSDKRevision;

		// Get the file version number generate by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	}
}

void FBXImporterImpl::UnInitialize()
{
	if (m_pFbxManger)
	{
		m_pFbxManger->Destroy();
		m_pFbxManger = nullptr;
	}

	if (GImporter)
	{
		delete GImporter;
		GImporter = nullptr;
	}
}

FBXFile *FBXImporterImpl::Import(const char *fbxFilename)
{
	if (!m_pFbxManger)
	{
		Initialize();
	}

	if (!m_pFbxManger)
	{
		return nullptr;
	}

	if (!InitFbxScene(fbxFilename))
	{
		return nullptr;
	}

	FbxPose *pose = nullptr;
	int NumGeometryCount = 0;
	std::vector<FbxNode*> LinkNodes;

	if (m_pScene->GetPoseCount() > 0)
	{
		pose = m_pScene->GetPose(0);
	}

	for (int i = 0; i < m_pScene->GetGeometryCount(); ++i)
	{
		FbxGeometry *Geometry = m_pScene->GetGeometry(i);

		if (Geometry->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxNode *GeoNode = Geometry->GetNode();
			if (!GeoNode)
			{
				continue;
			}

			size_t k = 0;
			for (; k < LinkNodes.size() && LinkNodes[k] != GeoNode; ++k);
			if (k >= LinkNodes.size())
			{
				LinkNodes.push_back(GeoNode);
			}
			
			ImportMesh((FbxMesh *)Geometry);
		}
	}

	return nullptr;
}

void FBXImporterImpl::ImportMesh(FbxMesh *fbxMesh)
{
	FbxNode *node = fbxMesh->GetNode();
	std::string name = node->GetName();

	int triCount = fbxMesh->GetPolygonCount();
	int numControlPoints = fbxMesh->GetControlPointsCount();

	if (!fbxMesh->IsTriangleMesh())
	{
		return;
	}

	fbxMesh->RemoveBadPolygons();

	int layerCount = fbxMesh->GetLayerCount();
	if (layerCount <= 0)
	{
		return;
	}
	
	//»ñµÃ uv ²ãÊý
	std::vector<std::string> UVSets;
	GetFbxMeshUVSet(fbxMesh, UVSets);
}

void FBXImporterImpl::GetFbxMeshUVSet(FbxMesh *fbxMesh, std::vector<std::string> &UVSets)
{
	int LayerCount = fbxMesh->GetLayerCount();
	if (LayerCount > 0)
	{
		for (int UVLayerIndex = 0; UVLayerIndex < LayerCount; ++UVLayerIndex)
		{
			FbxLayer *layer = fbxMesh->GetLayer(UVLayerIndex);
			int UVSetCount = layer->GetUVSetCount();
			if (UVSetCount > 0)
			{
				FbxArray<FbxLayerElementUV const*> ElUVs = layer->GetUVSets();
				for (int UVIndex = 0; UVIndex < UVSetCount; ++UVIndex)
				{
					FbxLayerElementUV const * ElementUV = ElUVs[UVIndex];
					if (ElementUV != NULL)
					{
						std::string UVName = ElementUV->GetName();
						UVSets.push_back(UVName);
					}
				}
			}
		}
	}
}

void FBXImporterImpl::GetFbxMeshMaterials(FbxMesh *fbxMesh)
{
	FbxNode * node = fbxMesh->GetNode();
	int MaterialCount = node->GetMaterialCount();

	for (int MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
	{
		FbxSurfaceMaterial *fbxMaterial = node->GetMaterial(MaterialIndex);
		ImportNaterial(fbxMaterial);
	}
}

void FBXImporterImpl::ImportNaterial(FbxSurfaceMaterial *fbxMaterial)
{
	int texSlots[ETexSlot::eMax] = { -1 };

	//diffuse map  ok
	FbxProperty fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string DiffuseMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eDiffuse] = ImportTexture(ETexSlot::eDiffuse, DiffuseMap);
		}
	}

	//specular map  ok
	fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string SpecularMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eSpecular] = ImportTexture(ETexSlot::eSpecular, SpecularMap);
		}
	}

	//emissive map  ok
	fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string EmissiveMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eEmissive] = ImportTexture(ETexSlot::eEmissive, EmissiveMap);
		}
	}

	//opacity map ok
	fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string AlphaMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eOpacity] = ImportTexture(ETexSlot::eOpacity, AlphaMap);
		}
	}

	//roughness map 
	fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string RoughnessMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eRoughness] = ImportTexture(ETexSlot::eRoughness, RoughnessMap);
		}
	}

	//metallic map
	fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string MatallicMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eMetallic] = ImportTexture(ETexSlot::eMetallic, MatallicMap);
		}
	}

	//normal map 
	fbxProperty = fbxMaterial->FindProperty("Bump");
	if (fbxProperty.IsValid())
	{
		int LayeredTextureCount = fbxProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxFileTexture::ClassId));
		if (LayeredTextureCount > 0)
		{
			FbxFileTexture *fbxTexture = (FbxFileTexture*)fbxProperty.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId), 0);
			std::string NormalMap = fbxTexture->GetRelativeFileName();
			texSlots[ETexSlot::eNormal] = ImportTexture(ETexSlot::eNormal, NormalMap);
		}
	}
}

int FBXImporterImpl::ImportTexture(ETexSlot slot, const std::string &texFilename)
{
	return -1;
}

bool FBXImporterImpl::InitFbxScene(const char *fbxFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;

	bool bOk = true;

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	m_pScene = FbxScene::Create(m_pFbxManger, "My Scene");
	if (!m_pScene)
	{
		bOk = false;
		goto finish;
	}

	// Create an importer.
	m_Importer = FbxImporter::Create(m_pFbxManger, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = m_Importer->Initialize(fbxFilename, -1, m_pFbxManger->GetIOSettings());
	m_Importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
	if (!lImportStatus)
	{
		bOk = false;
		goto finish;
	}

	if (!m_Importer->Import(m_pScene))
	{
		FbxGeometryConverter converter(m_pFbxManger);
		converter.Triangulate(m_pScene, true);
		bOk = false;
		goto finish;
	}

finish:
	if (!bOk)
	{
		DestroyFbxScene();
	}

	return bOk;
}

void FBXImporterImpl::DestroyFbxScene()
{
	if (m_Importer)
	{
		m_Importer->Destroy();
		m_Importer = NULL;
	}

	if (m_pScene)
	{
		m_pScene->Destroy();
		m_pScene = NULL;
	}
}

#endif


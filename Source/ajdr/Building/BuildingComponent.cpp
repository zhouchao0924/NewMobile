
#include "ajdr.h"
#include "ISuite.h"
#include "IProperty.h"
#include "IMeshObject.h"
#include "ResourceMgr.h"
#include "SurfaceFile.h"
#include "BuildingComponent.h"
#include "Building/BuildingConfig.h"

UBuildingComponent::UBuildingComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBuildingComponent::SetData(UBuildingData *InData)
{
	ClearAllMeshSections();

	Data = InData;

	float *pVertices = nullptr;
	float *pNors = nullptr;
	float *pUVs = nullptr;
	float *pTans = nullptr;
	float *pLightmapUVs = nullptr;
	int	   nVtxs = 0, nIndices = 0;
	int	   *pIndices = nullptr;

	IObject *RawObj = Data->GetRawObj();
	if (RawObj && RawObj->IsA(EPrimitive))
	{
		int32 MeshCount = RawObj->GetMeshCount();

		for (int32 i = 0; i < MeshCount; ++i)
		{
			IMeshObject *MeshObj = RawObj->GetMeshObject(i);
			int SectionCount = MeshObj->GetSectionCount();
			for (int32 iSection = 0; iSection < SectionCount; ++iSection)
			{
				if (MeshObj->GetSectionMesh(iSection, pVertices, pNors, pTans, pUVs, pLightmapUVs, nVtxs, pIndices, nIndices))
				{
					TArray<FVector> Vertices, Normals;
					TArray<int32> Triangles;
					TArray<FVector2D> UVs;
					TArray<FColor> VertexColors;
					TArray<FProcMeshTangent> Tangents;

					UVs.SetNum(nVtxs);
					Vertices.SetNum(nVtxs);
					Normals.SetNum(nVtxs);
					Tangents.SetNum(nVtxs);
					VertexColors.SetNum(nVtxs);
					Triangles.SetNum(nIndices);

					FMemory::Memcpy(Vertices.GetData(), pVertices, nVtxs * 12);
					FMemory::Memcpy(Normals.GetData(), pNors, nVtxs * 12);

					for (int32 iVert = 0; iVert < Tangents.Num(); ++iVert)
					{
						VertexColors[iVert] = FColor::White;
						Tangents[iVert].TangentX = *((FVector *)(pTans + 3 * iVert));
					}

					FMemory::Memcpy(UVs.GetData(), pUVs, nVtxs * 8);
					FMemory::Memcpy(Triangles.GetData(), pIndices, nIndices * 4);

					CreateMeshSection(iSection, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, false);

					ISuite *Suite = InData->GetSuite();
					if (Suite)
					{
						ObjectID SurfaceID = Suite->GetSurface(Data->GetID(), iSection);
						FString MaterialPath = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
						int32 MaterialType = Suite->GetProperty(SurfaceID, "MaterialType").IntValue();
						if (MaterialPath.Len() > 0)
						{
							switch (MaterialType)
							{
							case 0:
							{
								UMaterialInterface *MaterialObj = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
								if (MaterialObj)
								{
									SetMaterial(iSection, MaterialObj);
								}
								break;
							}
							case 1:
							{
								USurfaceFile *Surface = Cast<USurfaceFile>(UResourceMgr::GetResourceMgr()->FindRes(MaterialPath, true));
								if (Surface)
								{
									Surface->ForceLoad();
									UMaterialInterface *MaterialObj = Surface->GetUE4Material();
									if (MaterialObj)
									{
										SetMaterial(iSection, MaterialObj);
									}
								}
							}
							default: break;
							}
						}
						else
						{
							FBuildingConfig *Config = InData->GetConfig();
							if (Config)
							{
								SetMaterial(iSection, Config->WallMaterial);
							}
						}
					}
				}
			}
		}

		SetCollisionProfileName(TEXT("Architect"));
	}
}

void UBuildingComponent::UpdateSurface(int32 SectionIndex, int32 SurfaceID)
{
	ISuite *Suite = Data ? Data->GetSuite() : nullptr;
	if (Suite)
	{
		UMaterialInterface *UE4Material = nullptr;
		FString MaterialPath = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
		int32 MaterialType = Suite->GetProperty(SurfaceID, "MaterialType").IntValue();
		if (MaterialPath.Len() > 0)
		{
			switch (MaterialType)
			{
			case EUE4Material:
			{
				UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
				break;
			}
			case ERefMaterial:
			{
				USurfaceFile *Surface = Cast<USurfaceFile>(UResourceMgr::GetResourceMgr()->FindRes(MaterialPath, true));
				if (Surface)
				{
					Surface->ForceLoad();
					UE4Material = Surface->GetUE4Material();
				}
			}
			default: break;
			}
		}
		else
		{
			FBuildingConfig *Config = Data ? Data->GetConfig() : nullptr;
			if (Config)
			{
				UE4Material = Config->WallMaterial;
			}
		}

		IObject *pSurface = Suite->GetObject(SurfaceID);

		UMaterialInstanceDynamic *MaterialInst = CreateDynamicMaterialInstance(SectionIndex, UE4Material);
		if (MaterialInst && pSurface)
		{
			IValue *pValue = pSurface->GetValueArray();
			if (pValue)
			{
				int32 numFields = pValue->GetNumFields();
				for (int32 i = 0; i < numFields; ++i)
				{
					IValue &ParamValue = pValue->GetField(i);
					const char *ParamName = pValue->GetFieldName(i);
					EVarType Type = ParamValue.GetType();
					switch (Type)
					{
					case kV_Float:
					{
						MaterialInst->SetScalarParameterValue(ANSI_TO_TCHAR(ParamName), ParamValue.FloatValue());
						break;
					}
					case kV_Vec4D:
					{
						kVector4D Value = ParamValue.Vec4Value();
						MaterialInst->SetVectorParameterValue(ANSI_TO_TCHAR(ParamName), FLinearColor(FORCE_TYPE(FVector4, Value)));
						break;
					}
					default:break;
					}
				}
			}
		}
	}
}

int32 UBuildingComponent::HitTestSurface(const FVector &RayStart, const FVector &RayDir) const
{
	ObjectID SurfaceID = INVALID_OBJID;
	IObject *pObj = Data->GetRawObj();
	if (pObj && pObj->HitTest(FORCE_TYPE(kVector3D, RayStart), FORCE_TYPE(kVector3D, RayDir), SurfaceID))
	{
		return SurfaceID;
	}
	return SurfaceID;
}




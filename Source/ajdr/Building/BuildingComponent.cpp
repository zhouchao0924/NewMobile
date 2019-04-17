
#include "ajdr.h"
#include "ISuite.h"
#include "IProperty.h"
#include "DRGameMode.h"
#include "IMeshObject.h"
#include "ResourceMgr.h"
#include "SurfaceFile.h"
#include "BuildingComponent.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Building/BuildingConfig.h"
#include "BuildingSystem.h"


UBuildingComponent::UBuildingComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	HitSurfaceID  = INVALID_OBJID;
}

void UBuildingComponent::SetData(UBuildingData *InData)
{
	ClearAllMeshSections();

	Data = InData;
	if (Data)
	{
		float *pVertices = nullptr;
		float *pNors = nullptr;
		float *pUVs = nullptr;
		float *pTans = nullptr;
		float *pLightmapUVs = nullptr;
		int	   nVtxs = 0, nIndices = 0;
		int	   *pIndices = nullptr;

		IObject *RawObj = Data->GetRawObj();
		if(!RawObj)		return;
		IMeshObject *MeshObj = RawObj ? RawObj->GetMeshObject(0) : nullptr;
		if (MeshObj)
		{
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

					CreateMeshSection(iSection, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

					ISuite *Suite = Data->GetSuite();
					if (Suite)
					{
						ObjectID SurfaceID = Suite->GetSurface(Data->GetID(), iSection);
						UpdateSurface(iSection, SurfaceID);
					}
				}
			}

			SetCollisionProfileName(TEXT("Architect"));					
		}

		bool Visible = Data->GetBool(TEXT("bVisible"));
		SetHiddenInGame(!Visible);
	}
}

void UBuildingComponent::UpdateSurface(int32 SectionIndex, int32 SurfaceID)
{
	UMaterialInterface* UE4Mat = GetMaterialInterfaceBySurfaceID(SurfaceID);
	AutoSetMatParams(SectionIndex, SurfaceID, UE4Mat);

	if (OnNotifyUpdateSurface.IsBound())
	{
		OnNotifyUpdateSurface.Broadcast(SectionIndex, SurfaceID, ComponetType);
	}
	/*
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
					if(MaterialPath.Len() > 0)
					{ 
						UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
					}
					break;
				}
				case ERefMaterial:
				{
					UResource *Resource = UResourceMgr::GetResourceMgr()->FindRes(MaterialPath, true);
					if(Resource)
					{
						Resource->ForceLoad();
					}
					USurfaceFile *Surface = Cast<USurfaceFile>(Resource);
					if (Surface)
					{
						UE4Material = Surface->GetUE4Material();
					}
					UModelFile *ModelFile = Cast<UModelFile>(Resource);
					if (ModelFile)
					{
						UE4Material = ModelFile->GetUE4Material(0);
					}
				}
				default: break;
			}
		}
		else
		{
			FBuildingConfig *Config = Data? Data->GetConfig() : nullptr;
			if (Config)
			{
				UE4Material = Config->WallMaterial;
			}
			else
			{
				UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *DEFAULT_WALL_MATERIAL));
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
							kVector4D Value2 = ParamValue.Vec4Value();
							MaterialInst->SetVectorParameterValue(ANSI_TO_TCHAR(ParamName), FLinearColor(FORCE_TYPE(FVector4,Value2)));
							break;
						}
						default:break;
					}
				}
			}
		}
	}
	*/
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

int32 UBuildingComponent::HitTestSubModelIndex(const FVector &RayStart, const FVector &RayDir)
{
	int SubModelIndex = INVALID_OBJID;
	if(Data)
	{
		IObject *RawObj = Data->GetRawObj();
		IMeshObject *MeshObj = RawObj ? RawObj->GetMeshObject(0) : nullptr;
		if (MeshObj && MeshObj->HitTest(FORCE_TYPE(kVector3D, RayStart), FORCE_TYPE(kVector3D, RayDir), &SubModelIndex, nullptr, nullptr))
		{
			//return SubModelIndex;
		}
	}
	return SubModelIndex;
}

void UBuildingComponent::SetSaveUVData(const FVector2D& UV, const FVector2D& UVOffset, const float& Angle)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		if (Suite && BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{	
				kPoint kUV = ToBuildingPosition(UV);
				IValue& UVValue = VF->Create(&kUV, true);
				kPoint kUVOffset = ToBuildingPosition(UVOffset);
				IValue& UVOffsetValue = VF->Create(&kUVOffset, true);
				IValue& AngleValue = VF->Create(Angle);
				Suite->SetProperty(Data->GetID(), "UV", &UVValue);
				Suite->SetProperty(Data->GetID(), "UVOffset", &UVOffsetValue);
				Suite->SetProperty(Data->GetID(), "Angle", &AngleValue);
				//UpdateSurface(SubModelIndex, SurfaceID);
			}
		}
	}
}

void UBuildingComponent::SetMatFloatParam(FString& ParamName, const float& Value)
{
	 if(Data)
	 {
		 UBuildingSystem* BS = Data->GetBuildingSystem();
		 if ( BS)
		 {
			 IValueFactory* VF = BS->GetValueFactory();
			 if(VF)
			 {
				 IValue& NewValue = VF->Create(Value);
				 SetMatParam(HitSurfaceID, ParamName, &NewValue);
			 }	
		 }
	 }
}
/*
void UBuildingComponent::SetMat2DParam(FString& ParamName, const FVector2D& Value)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		if (BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				kPoint kValue = ToBuildingPosition(Value);
				IValue& NewValue = VF->Create(&kValue, true);
				SetMatParam(HitSurfaceID, ParamName, NewValue);
			}
		}
	}
}

void UBuildingComponent::SetMat3DParam(FString& ParamName, const FVector& Value)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		if (BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				kVector3D kValue = ToBuildingVector(Value);
				IValue& NewValue = VF->Create(&kValue, true);
				SetMatParam(HitSurfaceID, ParamName, NewValue);
			}
		}
	}
}*/

void UBuildingComponent::SetMat4DParam(FString& ParamName, const FVector4& Value)
{
	if (Data)
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		if (BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				kVector4D kValue = ToBuildingVector4D(Value);
				IValue& NewValue = VF->Create(&kValue, true);
				SetMatParam(HitSurfaceID, ParamName, &NewValue);
			}
		}
	}
}

void UBuildingComponent::SetMatDefaultParams(const int32& SurfaceID)
{
	HitSurfaceID = SurfaceID;
	UMaterialInterface* UE4Mat = GetMaterialInterfaceBySurfaceID(SurfaceID);
	
	FString U = TEXT("密度U");
	float UDefault = GetMatDefaultFloatParam(UE4Mat, U);
	if (UDefault <= 0)
	{
		UDefault = 1.0;
	}
	FString V = TEXT("密度V");
	float VDefault = GetMatDefaultFloatParam(UE4Mat, V);
	if (VDefault <= 0)
	{
		VDefault = 1.0;
	}
	UVDefault = FVector2D(UDefault, VDefault);
	UE_LOG(LogTemp, Log, TEXT("Default U = %f, V = %f"), UDefault, VDefault);
	FString NU = TEXT("法线U");
	FString NV = TEXT("法线V");
	FString UOffset = TEXT("位移X");
	FString VOffset = TEXT("位移Y");
	FString TAngle = TEXT("贴图角度");
	FString NAngle = TEXT("法线角度");
	SetMatFloatParam(U, 1.0 * UDefault);
	SetMatFloatParam(V, 1.0 * VDefault);
	SetMatFloatParam(NU, 1.0 * UDefault);
	SetMatFloatParam(NV, 1.0 * VDefault);
	SetMatFloatParam(UOffset, 0.0);
	SetMatFloatParam(VOffset, 0.0);
	SetMatFloatParam(TAngle, 0.0);
	SetMatFloatParam(NAngle, 0.0);
}

void UBuildingComponent::SetMatParam(const int32& SurfaceID, const FString& ParamName, IValue* Value)
{
	if(Data && Data->GetSuite())
	{	
		ISuite* Suite = Data->GetSuite();
		IObject *pSurface = Suite->GetObject(SurfaceID);
		if(pSurface)
		{
			pSurface->SetPropertyValue(TCHAR_TO_ANSI(*ParamName), Value);
		}
	}

}

void UBuildingComponent::SetMatParams(const int32& SurfaceID, const FVector2D& UVScale, const FVector2D& UVOffset, const float& Angle, const FVector2D& DefaultUV)
{
	 if(Data && (SurfaceID != INVALID_OBJID))
	 {
		UBuildingSystem* BS = Data->GetBuildingSystem();
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		if (Suite && BS && VF)
		{
			IObject *pSurface = Suite->GetObject(SurfaceID);
			if (pSurface)
			{
				IValue& Value = VF->Create();
				kPoint KUVScale = ToBuildingPosition(UVScale);
				IValue& ScaleV = VF->Create(&KUVScale, true);
				Value.AddField("UVScale", ScaleV);
				kPoint KUVOffset = ToBuildingPosition(UVOffset);
				IValue& OffsetV = VF->Create(&KUVOffset, true);
				Value.AddField("UVOffset", OffsetV);
				kPoint KDefaultUV = ToBuildingPosition(DefaultUV);
				IValue& DefaultUVV = VF->Create(&KDefaultUV, true);
				Value.AddField("UVDefault", DefaultUVV);
				Value.AddField("Angle", VF->Create(Angle));

				pSurface->SetValue("MatParam", &Value);
			}	
		}
	 }
}

void UBuildingComponent::GetMatParams(const int32& SurfaceID, FVector2D& UVScale, FVector2D& UVOffset, float& Angle, FVector2D& DefaultUV)
{
	if (Data && (SurfaceID != INVALID_OBJID))
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		if(BS && Suite)
		{
			IObject *pSurface = Suite->GetObject(SurfaceID);
			if (pSurface)
			{
				IValue& Value = pSurface->GetPropertyValue("MatParam");
				kPoint kUVScale = Value.GetField("UVScale").Vec2Value();
				UVScale = ToUE4Vector2D(kUVScale);
				kPoint kUVOffset = Value.GetField("UVOffset").Vec2Value();
				UVOffset = ToUE4Vector2D( kUVOffset );
				kPoint kUVDefault = Value.GetField("UVDefault").Vec2Value();
				UVDefault = ToUE4Vector2D(kUVDefault);
				Angle = Value.GetField("Angle").FloatValue();
			}

		}
	}
}

void UBuildingComponent::SetNewMaterial(const FVector &RayStart, const FVector &RayDir, const int32& MaterialType, const int32& ModelID, UPARAM(ref)FString& ResID)
{
	int32 SurfaceID = HitTestSurface(RayStart, RayDir);
	HitSurfaceID = SurfaceID;
	int32 SubModelIndex = HitTestSubModelIndex(RayStart, RayDir);
	UE_LOG(LogTemp, Log, TEXT("HitTest SurfaceID = %d, SubModelIndex = %d"), SurfaceID, SubModelIndex);
	if (Data && (SurfaceID != INVALID_OBJID))
	{
		UBuildingSystem* BS = Data->GetBuildingSystem();
		ISuite *Suite = Data ? Data->GetSuite() : nullptr;
		if (Suite && BS)
		{
			IValueFactory* VF = BS->GetValueFactory();
			if (VF)
			{
				IValue& ModelIDValue = VF->Create(ModelID);
				IValue& ResIDValue = VF->Create(TCHAR_TO_UTF8(*ResID), true);
				IValue& MatTypeValue = VF->Create(int(EMaterialType::EMaterialRef));
				//Suite->SetValue(SurfaceID, "ModelID", &ModelIDValue);
				Suite->SetProperty(SurfaceID, "MaterialType", &MatTypeValue);
				Suite->SetProperty(SurfaceID, "Material", &ResIDValue);
				SetMatDefaultParams(SurfaceID);
				UpdateSurface(SubModelIndex, SurfaceID);	
			}
			//FString MaterialPath = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
			// int32 MaterialType = Suite->GetProperty(SurfaceID, "MaterialType").IntValue();
		}
	}
}

UMaterialInterface * UBuildingComponent::GetMaterialInterfaceBySurfaceID(const int32& SurfaceID)
{
	ISuite *Suite = Data ? Data->GetSuite() : nullptr;
	UMaterialInterface *UE4Material = nullptr;
	if (Suite)
	{
		FString MaterialPath = ANSI_TO_TCHAR(Suite->GetProperty(SurfaceID, "Material").StrValue());
		int32 MaterialType = Suite->GetProperty(SurfaceID, "MaterialType").IntValue();
		if (MaterialPath.Len() > 0)
		{
			switch (MaterialType)
			{
			case EUE4Material:
			{
				if (MaterialPath.Len() > 0)
				{
					UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *MaterialPath));
				}
				break;
			}
			case ERefMaterial:
			{
				UResource *Resource = UResourceMgr::GetResourceMgr()->FindRes(MaterialPath, true);
				if (Resource)
				{
					Resource->ForceLoad();
				}
				USurfaceFile *Surface = Cast<USurfaceFile>(Resource);
				if (Surface)
				{
					UE4Material = Surface->GetUE4Material();
				}
				UModelFile *ModelFile = Cast<UModelFile>(Resource);
				if (ModelFile)
				{
					UE4Material = ModelFile->GetUE4Material(0);
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
			else
			{
				UE4Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *DEFAULT_WALL_MATERIAL));
			}
		}
	}
	//UMaterialInstanceDynamic *MaterialInst = CreateDynamicMaterialInstance(SectionIndex, UE4Material);
	return UE4Material;
}

void UBuildingComponent::AutoSetMatParams(const int32& SectionIndex, const int32& SurfaceID, UMaterialInterface *UE4Mat)
{
	ISuite *Suite = Data ? Data->GetSuite() : nullptr;
	if(!Suite)	return;
	IObject *pSurface = Suite->GetObject(SurfaceID);

	UMaterialInstanceDynamic *MaterialInst = CreateDynamicMaterialInstance(SectionIndex, UE4Mat);
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
					kVector4D Value2 = ParamValue.Vec4Value();
					MaterialInst->SetVectorParameterValue(ANSI_TO_TCHAR(ParamName), FLinearColor(FORCE_TYPE(FVector4, Value2)));
					break;
				}
				default:break;
				}
			}
		}
	}
}

float UBuildingComponent::GetMatDefaultFloatParam(const UMaterialInterface *UE4Mat, const FString& ParamName)
{
	float Value;
	const TCHAR* TParamName = *ParamName;
	FMaterialParameterInfo FPI(TParamName);
	UE4Mat->GetScalarParameterValue(FPI, Value);
	return Value;
}


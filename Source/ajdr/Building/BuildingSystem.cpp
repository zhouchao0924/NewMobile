
#include "ajdr.h"
#include "BuildingSystem.h"
#include "ModelFile.h"
#include "ModelFileComponent.h"
#include "ResourceMgr.h"
#include "SlateMaterialBrush.h"
#include "BuildingSDK.h"
#include "IBuildingSDK.h"
#include "Math/kMatrix43.h"
#include "Building.h"
#include "BuildingActor.h"
#include "ModelFileActor.h"
#include "BuildingComponent.h"
#include "Math/kVector2D.h"
#include <vector>
#include "SceneViewport.h"
#include "Public/UObject/ConstructorHelpers.h"
#include "DRGameMode.h"
#include "FileHelper.h"
#include "PointLightActor.h"
#include "SpotLightActor.h"
#include "DWActor.h"


IBuildingSDK *UBuildingSystem::BuildingSDK = nullptr;

IBuildingSDK *UBuildingSystem::GetBuildingSDK()
{
	if (!BuildingSDK)
	{
		FBuildingSDKModule &SDKModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
		BuildingSDK = SDKModule.GetSDK();
	}
	return BuildingSDK;
}

IValueFactory *UBuildingSystem::GetValueFactory()
{
	IBuildingSDK *SDK = GetBuildingSDK();
	if (SDK)
	{
		return SDK->GetValueFactory();
	}
	return nullptr;
}

UBuildingSystem::UBuildingSystem(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, Suite(nullptr)
	, Visitor(nullptr)
	, DWHasLoad(false)

{
	/*static ConstructorHelpers::FClassFinder<AActor> PointLight(TEXT("Blueprint'/Game/Light/Editor_PointLight_Base.Editor_PointLight_Base_C'"));
	UClass *PointLightClass = PointLight.Class;*/
}
//载入文本文件
bool UBuildingSystem::LoadStringFromFile(FString& Contents, FString FullFilePath)
{

	FString File;

	if (FullFilePath == "" || FullFilePath == " ") return false;

	if (!FFileHelper::LoadFileToString(File, *FullFilePath, FFileHelper::EHashOptions::None))
	{
		return false;
	}

	Contents = File;
	return true;

}


UBuildingSystem *UBuildingSystem::LoadNewSuite(UObject *Outer, const FString &InFilename)
{
	UBuildingSystem *NewSuite = NewObject<UBuildingSystem>(Outer);
	if (NewSuite)
	{
		NewSuite->LoadFile(InFilename);
	}
	return NewSuite;
}

UBuildingSystem *UBuildingSystem::CreateNewSuite(UObject *Outer, const FString &InFilename)
{
	IBuildingSDK *pSDK = GetBuildingSDK();
	UBuildingSystem *NewSuite = nullptr;
	if (pSDK)
	{
		NewSuite = NewObject<UBuildingSystem>(Outer);
		if (NewSuite)
		{
			NewSuite->Filename = InFilename;
			NewSuite->Suite = pSDK->CreateSuite();
			NewSuite->Suite->SetListener(NewSuite);
		}
	}
	return NewSuite;
}

bool UBuildingSystem::LoadFile(const FString &InFilename)
{
	if (!IFileManager::Get().FileExists(*InFilename))
	{
		return false;
	}
	if (Suite)
	{
		ClearSuite();
	}

	const char *strFilename = TCHAR_TO_ANSI(*InFilename);
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (pSDK)
	{
		Suite = pSDK->LoadSuite(strFilename);
		if (Suite)
		{
			Filename = InFilename;
			Suite->SetListener(this);
			LoadObjInfo();
			return true;
		}
	}
	return false;
}

void UBuildingSystem::SaveFile(const FString &InFilename)
{
	if (Suite)
	{
		const char *strFilename = TCHAR_TO_ANSI(*InFilename);
		Suite->Save(strFilename);
	}
}

void UBuildingSystem::ClearSuite()
{
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (Suite && pSDK)
	{
		pSDK->DestroySuite(Suite);
		Suite = NULL;
	}

	for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
	{
		FObjectInfo &ObjInfo = It.Value();
		ClearObjInfo(&ObjInfo);
	}
	
	ObjMap.Empty();
}

int32 UBuildingSystem::FindHostWorld(UWorld *World)
{
	int32 FoundHostIndex = INDEX_NONE;
	if (World)
	{
		for (int32 i = HostWorlds.Num() - 1; i >= 0; --i)
		{
			UObject *Obj = HostWorlds[i].Get(true);
			if (Obj == NULL)
			{
				HostWorlds.RemoveAt(i);
			}
			else if (Obj == World)
			{
				FoundHostIndex = i;
				break;
			}
		}
	}
	return FoundHostIndex;
}

void UBuildingSystem::AddToWorld(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : NULL;
	if (MyWorld)
	{
		int32 HostIndex = FindHostWorld(MyWorld);
		if (HostIndex==INDEX_NONE && Suite)
		{
			for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
			{
				FObjectInfo &ObjInfo = It.Value();
				//SpawnActorByObject(MyWorld, ObjInfo);
			}
			HostWorlds.Add(MyWorld);
		}
	}
}

void UBuildingSystem::RemoveFromWorld(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : NULL;
	if (MyWorld)
	{
		int32 HostIndex = FindHostWorld(MyWorld);
		if (HostIndex != INDEX_NONE)
		{
			for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
			{
				FObjectInfo &ObjInfo = It.Value();
				for (int32 i = ObjInfo.Actorts.Num()-1; i>=0; --i)
				{
					ADRActor *Actor = ObjInfo.Actorts[i];
					if (Actor)
					{
						if (Actor->GetWorld() == MyWorld)
						{
							Actor->Destroy();
							ObjInfo.Actorts.RemoveAt(i);
						}
					}
				}
			}
			HostWorlds.Remove(MyWorld);
		}
	}
}

void UBuildingSystem::SetRoomCeilVisible(bool bVisible)
{
	if (Suite)
	{
		Suite->SetRoomCeilVisible(bVisible);
	}
}

bool UBuildingSystem::IsRoomCeilVisible()
{
	if (Suite)
	{
		return Suite->IsRoomCeilVisible();
	}
	return false;
}

void UBuildingSystem::LoadObjInfo()
{
	IObject **ppObjects = nullptr;
	int nObjs = Suite->GetAllObjects(ppObjects);

	for (int i = 0; i < nObjs; ++i)
	{
		IObject *Obj = ppObjects[i];
		if (Obj)
		{
			int32 ID = Obj->GetID();
			check(ObjMap.Find(ID) == NULL);
			NewSuiteData(Obj);
		}
	}
}

FObjectInfo *UBuildingSystem::NewSuiteData(IObject *RawObj)
{
	FObjectInfo *ObjInfo = nullptr;

	if (RawObj)
	{
		UBuildingData *Data = NewObject<UBuildingData>(this);
		if (Data)
		{
			int32 ID = RawObj->GetID();
			Data->SetRawObj(ID);
			Data->BuildingSystem = this;
			ObjInfo = &ObjMap.Add(ID, FObjectInfo(Data));
		}
	}

	return ObjInfo;
}

void UBuildingSystem::SetSelected(ObjectID ObjID, bool bSelected)
{
	FObjectInfo *pObjInfo = ObjMap.Find(ObjID);
	if (pObjInfo)
	{
	}
}

void UBuildingSystem::OnAddObject(IObject *RawObj)
{
	if (Suite)
	{
		FObjectInfo *ObjInfo = NewSuiteData(RawObj);
		if (ObjInfo)
		{
			for (int32 i = HostWorlds.Num() - 1; i >= 0; --i)
			{
				UWorld *World = (UWorld *)HostWorlds[i].Get(true);
				if (World)
				{
					SpawnActorByObject(RawObj, World, *ObjInfo);
				}
				else
				{
					HostWorlds.RemoveAt(i);
				}
			}
		}
	}
}

void UBuildingSystem::OnDeleteObject(IObject *RawObj)
{
	if (Suite)
	{
		int32 ObjID = RawObj->GetID();
		FObjectInfo *ObjInfo = ObjMap.Find(ObjID);
		if (ObjInfo)
		{
			ClearObjInfo(ObjInfo);
			ObjMap.Remove(ObjID);
		}
	}
}

void UBuildingSystem::ClearObjInfo(FObjectInfo *ObjInfo)
{
	for (int32 i = 0; i < ObjInfo->Actorts.Num(); ++i)
	{
		ADRActor *Actor = ObjInfo->Actorts[i];
		UWorld *MyWorld = Actor ? Actor->GetWorld() : nullptr;
		if (MyWorld)
		{
			MyWorld->DestroyActor(Actor);
		}
	}

	ObjInfo->Actorts.Empty();

	if (ObjInfo->Data)
	{
		ObjInfo->Data->ConditionalBeginDestroy();
	}
}

void UBuildingSystem::OnUpdateObject(IObject *RawObj, unsigned int ChannelMask)
{
	if (Suite)
	{
		int32 ObjID = RawObj->GetID();
		FObjectInfo *ObjInfo = ObjMap.Find(ObjID);
		EObjectType TestType = RawObj->GetType();
		EChannelMask TestChannel = (EChannelMask)ChannelMask;
		if (ObjInfo)
		{
			for(int32 i = 0; i<ObjInfo->Actorts.Num(); ++i)
			{
				ABuildingActor *Actor = Cast<ABuildingActor>(ObjInfo->Actorts[i]);
				if (Actor)
				{
					Actor->Update(ObjInfo->Data);
				}
			}
			if ( RawObj->GetType() == EObjectType::EModelInstance)
			{
				int num = ObjInfo->Actorts.Num();
				for (int i = 0; i < num; ++i)
				{
					AModelFileActor *Model =  Cast<AModelFileActor>(ObjInfo->Actorts[i]);
					if (Model)
					{
						Model->Update(ObjInfo->Data);
					}
				}
			}
			if (RawObj->GetType() == EObjectType::EFloorPlane)
			{
				int num = ObjInfo->Actorts.Num();
				for (int i = 0; i < num; ++i)
				{
					ABuildingActor *Model = Cast<ABuildingActor>(ObjInfo->Actorts[i]);
					if (Model)
					{
						Model->Update(ObjInfo->Data);
					}
				}
			}
			if (RawObj->GetType() == EObjectType::EDoorHole
				|| RawObj->GetType() == EObjectType::EWindow)
			{

				ISuite * _IS = ObjInfo->Data->GetSuite();
				if (_IS)
				{
					IValue & Value = _IS->GetProperty(ObjID, "LinkObjects");
					kArray<int>  LinkObjects = Value.IntArrayValue();
					for (int i = 0; i < LinkObjects.size(); ++i)
					{
						FObjectInfo * _ObjInfo = ObjMap.Find(LinkObjects[i]);
						if (_ObjInfo)
						{
							AModelFileActor *Model = Cast<AModelFileActor>(_ObjInfo->Actorts[i]);
							if (Model)
							{
								Model->Update(ObjInfo->Data);
								Model->Update(_ObjInfo->Data);
							}
						}
					}
				}

			}
		}
	}
}

//根据Object的Type生成对应的部件
ADRActor *UBuildingSystem::SpawnActorByObject(IObject *RawObj, UWorld *World, FObjectInfo &ObjInfo)
{
	ADRActor *pActor = nullptr;
	IObject *Obj = ObjInfo.Data->GetRawObj();

	if (!pActor)
	{
		if (Obj->IsA(EPrimitive))
		{
			pActor = SpawnPrimitiveComponent(World, ObjInfo, Obj->GetType());
		}
		else if (Obj->IsA(EModelInstance))
		{
			pActor = SpawnModelComponent(RawObj, World, ObjInfo);
			if (!DWHasLoad)
			{
				SpawnDWModelComponent(World, ObjInfo);
			}
		}
		else
		{
			pActor = SpawnLightComponent(RawObj, World, ObjInfo);
		}
	}

	if (Visitor)
	{
		Visitor->OnCheckObjectVisible(ObjInfo);
	}
	return pActor;
}

void UBuildingSystem::SetADataList(const FADatac AData)
{
	if (Suite)
	{
		ADataList.Add(AData);
	}
}

void UBuildingSystem::SetLightDataList(const FLightDatac LData)
{
	if (Suite)
	{
		LightDataList.Add(LData);
	}
}

void UBuildingSystem::SetDWDataList(const FDWDatac DWData)
{
	if (Suite)
	{
		DWDataList.Add(DWData);
	}
}

//生成ModelFile模型
ADRActor * UBuildingSystem::SpawnModelComponent(IObject *RawObj, UWorld *MyWorld, FObjectInfo &ObjInfo)
{
	UBuildingData *Data = ObjInfo.Data;
	FTransform Tra;
	int n = 0;
	for (int i = 0; i < ADataList.Num(); ++i)
	{
		int32 a = ADataList[i].ObjID;
		int32 b = RawObj->GetID();
		if (a == b)
		{
			Tra.SetScale3D(ADataList[i].Scale);
			n = i;
			break;
		}
	};

	AModelFileActor *Actor = MyWorld->SpawnActor<AModelFileActor>(AModelFileActor::StaticClass(), Tra);
	if (Actor)
	{
		Actor->Update(ObjInfo.Data);

		FRotator RR;
		RR.Pitch = ADataList[n].Rotation.Yaw;
		RR.Yaw = ADataList[n].Rotation.Pitch;
		RR.Roll = ADataList[n].Rotation.Roll;
		Actor->SetActorRotation(RR);

		ObjInfo.Actorts.Add(Actor);
	}
	return Actor;
};

//生成门窗模型
void  UBuildingSystem::SpawnDWModelComponent(UWorld *MyWorld, FObjectInfo &ObjInfo)
{
	ADWActor *Actor = nullptr;
	FTransform Tra;
	for (int i = 0; i < DWDataList.Num(); ++i)
	{
		Tra.SetLocation(DWDataList[i].Loctioan);
		Tra.SetScale3D(DWDataList[i].Scale);
		ADWActor *Actor = MyWorld->SpawnActor<ADWActor>(ADWActor::StaticClass(), Tra);

		if (Actor)
		{			
			Actor->Update(DWDataList[i].ResID);

			FRotator RR;
			RR.Pitch = DWDataList[i].Rotation.Yaw;
			RR.Yaw = DWDataList[i].Rotation.Pitch;
			RR.Roll = DWDataList[i].Rotation.Roll;
			Actor->SetActorRotation(RR);

			ObjInfo.Actorts.Add(Actor);
		}
	};
	DWHasLoad = true;	
};
	
//生成户型组件
ADRActor * UBuildingSystem::SpawnPrimitiveComponent(UWorld *MyWorld, FObjectInfo &ObjInfo, int ObjectType)
{
	ABuildingActor *Actor = (ABuildingActor *)MyWorld->SpawnActor(ABuildingActor::StaticClass(), &FTransform::Identity);
	if (Actor)
	{
		Actor->bTopCeil = ObjectType == ECeilPlane ? 1 : 0;
		Actor->BuildingData = ObjInfo.Data;
		UBuildingComponent *WallComp = NewObject<UBuildingComponent>(Actor);
		if (WallComp)
		{		
			WallComp->RegisterComponentWithWorld(MyWorld);
			WallComp->SetData(ObjInfo.Data);		
		}
		ObjInfo.Actorts.Add(Actor);
	}
	return Actor;
}

//生成灯光组件
ADRActor *UBuildingSystem::SpawnLightComponent(IObject *RawObj,UWorld *MyWorld, FObjectInfo &ObjInfo)
{
	APointLightActor *Actor = nullptr;
	if (LightDataList.Num()>0)
	{
		for (int i = 0; i < LightDataList.Num(); ++i)
		{
			int32 a = LightDataList[i].ObjID;
			int32 b = RawObj->GetID();
			if (a == b)
			{	
				if (LightDataList[i].Type == 1)
				{
					APointLightActor *Actor = (APointLightActor *)MyWorld->SpawnActor(APointLightActor::StaticClass(), &FTransform::Identity);
					FVector Location = LightDataList[i].Location;
					Actor->SetActorRelativeLocation(Location);

					Actor->Update(ObjInfo.Data, LightDataList[i].SI);
					ObjInfo.Actorts.Add(Actor);
					break;
				}
				else
				{
					ASpotLightActor *Actor = (ASpotLightActor *)MyWorld->SpawnActor(ASpotLightActor::StaticClass(), &FTransform::Identity);
					FVector Location = LightDataList[i].Location;
					Actor->SetActorRelativeLocation(Location);

					Actor->Update(ObjInfo.Data, LightDataList[i].SI);
					ObjInfo.Actorts.Add(Actor);
					break;
				}				
			}
		};
	}
	return Actor;
}

bool UBuildingSystem::IsFree(int32 ObjID)
{
	return Suite ? Suite->IsFree(ObjID) : false;
}

int32 UBuildingSystem::GetAllObjects(IObject** &ppOutObject, EObjectType InClass,bool bIncludeDeriveType)
{
	if (Suite)
	{
		return Suite->GetAllObjects(ppOutObject, InClass, bIncludeDeriveType);
	}
	return 0;
}

IObject *UBuildingSystem::GetObject(int32 ObjID)
{
	UBuildingData *pData = GetData(ObjID);
	if (pData)
	{
		return pData->GetRawObj();
	}
	return nullptr;
}

void UBuildingSystem::DeleteObject(int32 ObjID)
{
	if (Suite)
	{
		Suite->DeleteObject(ObjID);
	}
}

int32 UBuildingSystem::Snap(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance)
{
	if (Suite)
	{
		return Suite->FindSnapLocation(FORCE_TYPE(kPoint, Loc), FORCE_TYPE(kPoint, BestLoc), BestID, Ignore, Torlerance);
	}
	return kESnapType::kENone;
}

void UBuildingSystem::Tick(float DeltaTime)
{
	if (Suite)
	{
		Suite->Update();
	}
}

void AddTri(const FVector2D &S, const FVector2D &E, const FVector2D &Offset2D, FCanvasUVTri *Triangles, const FLinearColor &Color)
{
	Triangles[0].V0_Pos = S - Offset2D;
	Triangles[0].V1_Pos = S + Offset2D;
	Triangles[0].V2_Pos = E + Offset2D;

	Triangles[0].V0_Color = Color;
	Triangles[0].V1_Color = Color;
	Triangles[0].V2_Color = Color;

	Triangles[1].V0_Pos = E + Offset2D;
	Triangles[1].V1_Pos = E - Offset2D;
	Triangles[1].V2_Pos = S - Offset2D;

	Triangles[1].V0_Color = Color;
	Triangles[1].V1_Color = Color;
	Triangles[1].V2_Color = Color;
}

void UBuildingSystem::OnPropertyChanged(UBuildingData *Data, const FString &PropName, const IValue &NewValue)
{
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*PropName);
		Suite->SetProperty(Data->GetID(), AnsiName, &NewValue);
		if (EventPropertyChangedHandler.IsBound())
		{
			EventPropertyChangedHandler.Broadcast(Data, PropName);
		}
	}
}

UBuildingData *UBuildingSystem::GetData(int32 ID)
{
	FObjectInfo *pObjInfo = ObjMap.Find(ID);
	if (pObjInfo)
	{
		return pObjInfo->Data;
	}
	return NULL;
}

int32  UBuildingSystem::AddCorner(const FVector2D &Location)
{
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		return Suite->AddCorner(Loc.X, Loc.Y);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddModelToObject(int32 BaseObjID, const FString &ResID, const FVector &Location, const FRotator &Rotation, const FVector &Scale, int Type /*= -1*/)
{
	if (Suite)
	{
		const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
		kVector3D Loc = ToBuildingPosition(Location);
		kVector3D Sca = ToBuildingPosition(Scale);
		kRotation Rot = ToBuildingRotation(Rotation);
		return Suite->AddModelToObject(BaseObjID, AnsiResID, Loc, Rot, Sca, Type);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddModelToAnchor(int32 AnchorID, const FString &ResID, const FVector &Location)
{
	if (Suite)
	{
		const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
		kVector3D Loc = ToBuildingVector(Location);
		return Suite->AddModelToAnchor(AnchorID, AnsiResID, Loc);
	}
	return INVALID_OBJID;
}

void UBuildingSystem::AddWall(int32 StartCorner, int32 EndCorner, float ThickLeft, float ThickRight, float Height)
{
	if (Suite)
	{
		Suite->AddWall(StartCorner, EndCorner, ThickLeft, ThickRight, Height);
	}
}

int32 UBuildingSystem::AddWindow(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height)
{
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		return Suite->AddWindow(WallID, Loc, zPos, Width, Height);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddDoor(int32 WallID, const FVector2D &Location, float Width, float Height, float zPos /*= 0*/)
{
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		return Suite->AddDoor(WallID, Loc, Width, Height, zPos);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddPointLight(const FVector &Location, float SourceRadius, float SoftSourceRadius, float SourceLength, float Intensity, FLinearColor LightColor, bool bCastShadow)
{
	if (Suite)
	{
		kVector3D Loc = ToBuildingVector(Location);
		kColor Color = ToBuildingColor(LightColor);
		return (int32)Suite->AddPointLight(Loc, SourceRadius, SoftSourceRadius, SourceLength, Intensity, Color, bCastShadow);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddSpotLight(const FVector &Location, const FRotator &Rotationn, float AttenuationRadius, float SourceRadius, float SoftSourceRadius, float SourceLength, float InnerConeAngle, float OuterConeAngle, float Intensity, FLinearColor LightColor, bool bCastShadow)\
{
	if (Suite)
	{
		kVector3D Loc = ToBuildingVector(Location);
		kRotation Rot = ToBuildingRotation(Rotationn);
		kColor Color =  ToBuildingColor(LightColor);
		return (int32)Suite->AddSpotLight(Loc, Rot, AttenuationRadius, SourceRadius, SoftSourceRadius, SourceLength, InnerConeAngle, OuterConeAngle, Intensity, Color, bCastShadow);
	}
	return INVALID_OBJID;
}

void UBuildingSystem::SetSkyLight(const FDRSkyLight & Sky, const FDirectionLight & Direction)
{
	if (Suite)
	{
		ObjectID ConfigID = Suite->GetConfig();
		IValue & _Sky = Suite->GetProperty(ConfigID, "SkyLight");
		int32 SkyID = _Sky.IntValue();
		UBuildingData * SkyData = GetData(SkyID);
		if (SkyData)
		{
			//(FDRSkyLight&*)(&Sky)->ID = SkyID;
			//(FDirectionLight*)(&Direction)->ID = SkyID;
			//Direction
			SkyData->SetVector(TEXT("DirectionLightRotation"), Direction.Rotation.Vector());
			SkyData->SetVector(TEXT("DirectionLightColor"), FVector(Direction.Color.R, Direction.Color.G, Direction.Color.B));
			SkyData->SetFloat(TEXT("DirectionLightIntensity"), Direction.Intensity);
			//Sky
			SkyData->SetFloat(TEXT("SkyLightIntensity"), Sky.Intensity);
			SkyData->SetFloat(TEXT("SkyLightAngle"), Sky.Angle);
			SkyData->SetVector(TEXT("SkyLightColor"), FVector(Sky.Color.R, Sky.Color.G, Sky.Color.B));
		}
	}
}

void UBuildingSystem::SetPostProcess(const FPostProcess & Post)
{
	if (Suite)
	{
		ObjectID ConfigID = Suite->GetConfig();
		IValue & _Pose =  Suite->GetProperty(ConfigID,"PostProcess");
		int32 PostID = _Pose.IntValue();
		UBuildingData * PostData =  GetData(PostID);
		if (PostData)
		{
			//Post.ID = PostID;
			PostData->SetFloat(TEXT("Saturation"), Post.Saturation);
			//PostData->SetFloat(TEXT("Constrast"), Post.Constrast);
			PostData->SetFloat(TEXT("BloomIntensity"), Post.BloomIntensity);
			PostData->SetFloat(TEXT("AmbientOcclusion"), Post.AmbientOcclusion);
			PostData->SetFloat(TEXT("AmbientOcclusionRadius"), Post.AmbientOcclusionRadius);
		}
	}
}

int32 UBuildingSystem::FindCloseWall(const FVector2D & Location, float Width, FVector2D & BestLoc, float Tolerance)
{
	if (Suite)
	{
		int32 ID = - 1;
		kPoint Loc = ToBuildingPosition(Location);
		kPoint kBestLoc;
		ID = Suite->FindCloseWall(Loc, Width, kBestLoc, Tolerance);
		BestLoc = ToVector2D(kBestLoc);
		return ID;
	}
	return int32();
}

void UBuildingSystem::Build()
{
	if (Suite)
	{
		Suite->BuildRooms();
	}
}

UWorld *UBuildingSystem::GetWorld() const
{
	UObject *Outer = GetOuter();
	return Outer ? Outer->GetWorld() : nullptr;
}

void UBuildingSystem::SetChannelVisbile(EVisibleChannel Channel, bool bVisible)
{
	for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
	{
		FObjectInfo &ObjInfo = It.Value();
		for (int32 i = 0; i < ObjInfo.Actorts.Num(); ++i)
		{
			ADRActor *pActor = ObjInfo.Actorts[i];
			if (pActor && pActor->GetChannel()==Channel)
			{
				pActor->SetActorHiddenInGame(!bVisible);
			}
		}
	}
}

void UBuildingSystem::SetVisitor(IBuildingVisitor *InVisitor)
{
	Visitor = InVisitor;
}

void UBuildingSystem::UpdateChannelVisible()
{
	if (Visitor)
	{
		for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
		{
			FObjectInfo &ObjInfo = It.Value();
			Visitor->OnCheckObjectVisible(ObjInfo);
		}
	}
}

void UBuildingSystem::SetObjIntValue(const int32& ObjID, FString& ValueName, const int32& IntValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			IValue &Value = VF->Create(IntValue);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	 }
}

void UBuildingSystem::SetObjFloatValue(const int32& ObjID, FString& ValueName, const int32& FloatValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			IValue &Value = VF->Create(FloatValue);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFStringValue(const int32& ObjID, FString& ValueName, FString& FStringValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			const char* valuechar = FStringToConstChar(FStringValue);
			IValue &Value = VF->Create(valuechar, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFVector(const int32& ObjID, FString& ValueName, FVector& FVectorValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kVector3D KValue = ToBuildingVector(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFVector2D(const int32& ObjID, FString& ValueName, FVector2D& FVectorValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kPoint KValue = ToBuildingPosition(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFVector4D(const int32& ObjID, FString& ValueName, FVector4& FVectorValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kVector4D KValue = ToBuildingVector4D(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::GetAllCornerActorLoction(TArray<FVector2D> &OutAllCornerActorLoction, int32 objectID)
{
	if (Suite)
	{
		IObject **ppAllCornerObject = nullptr;
		int nObjs = Suite->GetAllObjects(ppAllCornerObject, EObjectType::ECorner);
		for (int i = 0; i < nObjs; ++i)
		{
			if (ppAllCornerObject[i]&& ppAllCornerObject[i]->GetID()!= objectID)
				OutAllCornerActorLoction.Add(ToVector2D(ppAllCornerObject[i]->GetPropertyValue("Location").Vec2Value()));
		}
	}
}

ObjectID UBuildingSystem::HitTest(const FVector2D &Location)
{
	ObjectID num = INDEX_NONE;
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		num = Suite->HitTest(Loc);
	}
	return num;
}

ObjectID UBuildingSystem::IsHitCornner(const FVector2D &Location)
{
	ObjectID HitObj = INVALID_OBJID;
	if (Suite)
	{
		ObjectID BestID = INVALID_OBJID;
		kPoint BestLocation;
		kPoint Loc = ToBuildingPosition(Location);
		if (kEPt == Suite->FindSnapLocation(Loc, BestLocation, BestID, INVALID_OBJID, 5))
		{
			HitObj = BestID;
		}
	}
	return HitObj;
}

bool UBuildingSystem::Move(ObjectID objID, const FVector2D &DeltaMove)
{
	if (Suite)
	{
		kPoint Loc= ToBuildingPosition(DeltaMove);
		return Suite->Move(objID, Loc);
	}
	return false;
}

void UBuildingSystem::LoadingConfig(FBuildingConfig * Config)
{
	if (!Suite)
	{
		IBuildingSDK *pSDK = GetBuildingSDK();
		if (pSDK)
		{
			Suite = pSDK->CreateSuite();
			Suite->SetListener(this);
		}
	}
	if (Suite)
	{
		ObjectID ConfigID = Suite->GetConfig();
		Suite->SetProperty(ConfigID, "WallMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->WallMaterial->GetPathName())));
		Suite->SetProperty(ConfigID, "WallMaterialType", &GetValueFactory()->Create(Config->WallMaterialType));
		Suite->SetProperty(ConfigID, "FloorMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->FloorMaterial->GetPathName())));
		Suite->SetProperty(ConfigID, "FloorMaterialType", &GetValueFactory()->Create(Config->FloorMaterialType));
		Suite->SetProperty(ConfigID, "CeilMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->WallMaterial->GetPathName())));
		Suite->SetProperty(ConfigID, "CeilMaterialType", &GetValueFactory()->Create(Config->CeilMaterialType));
		Suite->SetProperty(ConfigID, "bCeilVisible", &GetValueFactory()->Create(Config->bCeilVisible));
		Suite->SetProperty(ConfigID, "Tolerance", &GetValueFactory()->Create(Config->Tolerance));
		Suite->SetProperty(ConfigID, "DefaultDoor", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultDoor)));
		Suite->SetProperty(ConfigID, "DefaultDoorFrame", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultDoorFrame)));
		Suite->SetProperty(ConfigID, "DefaultWindow", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultWindow)));
	}
}

//void UBuildingSystem::SetHouseImage(FDRHouseImage _HI)
//{
//	if (Suite)
//	{
//		ObjectID ConfigID = Suite->GetConfig();
//		UBuildingData * _BD = GetData(ConfigID);
//		if (_BD)
//		{
//			IObject * _Obj = _BD->GetRawObj();
//			if (_Obj)
//			{
//				IValue & HI = GetValueFactory()->Create();
//				/*CopyMode,SpinValue,SceneWorldDistance,Path,Scene0Location,SLocation,PlaneLocation,PlaneRotation,PlaneScale*/
//				HI.AddField("CopyMode", GetValueFactory()->Create(_HI.CopyMode));
//				HI.AddField("SpinValue", GetValueFactory()->Create(_HI.SpinValue));
//				HI.AddField("SceneWorldDistance", GetValueFactory()->Create(_HI.SceneWorldDistance));
//				HI.AddField("Path", GetValueFactory()->Create(TCHAR_TO_ANSI(*(_HI.Path))));
//				kVector3D Scene0Location = ToBuildingVector(_HI.Scene0Location);
//				HI.AddField("Scene0Location", GetValueFactory()->Create(&Scene0Location));
//				kVector3D SLocation = ToBuildingVector(_HI.SLocation);
//				HI.AddField("SLocation", GetValueFactory()->Create(&SLocation));
//				kVector3D PlaneLocation = ToBuildingVector(_HI.PlaneLocation);
//				HI.AddField("PlaneLocation", GetValueFactory()->Create(&PlaneLocation));
//				kVector3D PlaneRotation = ToBuildingVector(_HI.PlaneRotation);
//				HI.AddField("PlaneRotation", GetValueFactory()->Create(&PlaneRotation));
//				kVector3D PlaneScale = ToBuildingVector(_HI.PlaneScale);
//				HI.AddField("PlaneScale", GetValueFactory()->Create(&PlaneScale));
//
//				_Obj->SetValue("HouseImage", &HI);
//			}
//		}
//	}
//}
//
//
//void UBuildingSystem::GetHouseImage(FDRHouseImage & HouseImage)
//{
//	if (Suite)
//	{
//		ObjectID ConfigID = Suite->GetConfig();
//		UBuildingData * _BD = GetData(ConfigID);
//		if (_BD)
//		{
//			IObject* _Obj = _BD->GetRawObj();
//			if (_Obj)
//			{
//				IValue* HIData = _Obj->FindValue("HouseImage");
//				if (HIData)
//				{
//					HouseImage.CopyMode = HIData->GetField("CopyMode").BoolValue();
//					HouseImage.SpinValue = HIData->GetField("SpinValue").FloatValue();
//					HouseImage.SceneWorldDistance = HIData->GetField("SceneWorldDistance").FloatValue();
//					HouseImage.Path = HIData->GetField("Path").StrValue();
//					HouseImage.Scene0Location = ToUE4Vector(HIData->GetField("Scene0Location").Vec3Value());
//					HouseImage.SLocation = ToUE4Vector(HIData->GetField("SLocation").Vec3Value());
//					HouseImage.PlaneLocation = ToUE4Vector(HIData->GetField("PlaneLocation").Vec3Value());
//					HouseImage.PlaneRotation = ToUE4Vector(HIData->GetField("PlaneRotation").Vec3Value());
//					HouseImage.PlaneScale = ToUE4Vector(HIData->GetField("PlaneScale").Vec3Value());
//				}
//			}
//		}
//	}
//}


int32 UBuildingSystem::GetPolygon(int32 objID, TArray<FVector2D>& TPolygons, bool binner)
{
	int32 num = INVALID_OBJID;
	TPolygons.Empty();
	if (Suite)
	{
		if (objID != INVALID_OBJID)
		{
			kPoint *pPolygons = nullptr;
			num = Suite->GetPolygon(objID, pPolygons, binner);
			for (int i = 0; i < num; ++i)
			{
				TPolygons.Add(ToVector2D(pPolygons[i]));
			}
		}
	}
	return num;
}

int	UBuildingSystem::HitTestMinMax(const FVector2D &Min, const FVector2D &Max, int32 *&pResults)
{
	int32 num = INVALID_OBJID;
	if (Suite)
	{
		pResults = nullptr;
		kPoint LocMin= ToBuildingPosition(Min);
		kPoint LocMax = ToBuildingPosition(Max);
		num = Suite->HitTest(LocMin, LocMax, pResults);
	}
	return num;
}

int32 UBuildingSystem::GetRoomByLocation(const FVector2D &Loc)
{
	int32 Objid= INVALID_OBJID;
	if (Suite)
	{
		kPoint KPLoc= ToBuildingPosition(Loc);
		Objid=Suite->GetRoomByLocation(KPLoc);
	}
	return Objid;
}

void  UBuildingSystem::AddVirtualWall(int32 StartCorner, int32 EndCorner)
{
	if (Suite)
	{
		Suite->AddVirtualWall(StartCorner, EndCorner);
	}
}

bool UBuildingSystem::GetWallVector(int32 WallID, FVector2D &P0, FVector2D &P1, FVector2D &Right)
{
	bool IsSuccess = false;
	if (Suite)
	{
		kPoint KP0 = ToBuildingPosition(P0);
		kPoint KP1 = ToBuildingPosition(P1);
		kPoint KPR = ToBuildingPosition(Right);
		IsSuccess = Suite->GetWallVector(WallID, KP0, KP1, KPR);
		if(IsSuccess)
		{
			P0 = ToVector2D(KP0);
			P1 = ToVector2D(KP1);
			Right = ToVector2D(KPR);
		}
	}
	return IsSuccess;
}

int32 UBuildingSystem::GetConnectWalls(int32 objID, int32 *&pConnectedWalls)
{
	int32 num = INVALID_OBJID;
	if (Suite&&objID != INVALID_OBJID)
	{
		pConnectedWalls = nullptr;
		num = Suite->GetConnectWalls(objID, pConnectedWalls);
	}
	return num;
}

UPrimitiveComponent *UBuildingSystem::HitTestPrimitiveByCursor(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	UGameViewportClient *GameViewportClient = MyWorld ? MyWorld->GetGameViewport() : nullptr;
	FSceneViewport *Viewport = GameViewportClient? GameViewportClient->GetGameViewport() : nullptr;
	if (Viewport)
	{
		int32 MouseX = Viewport->GetMouseX();
		int32 MouseY = Viewport->GetMouseY();

		HActor *HitActor = HitProxyCast<HActor>(Viewport->GetHitProxy(MouseX, MouseY));
		if (HitActor)
		{
			return const_cast<UPrimitiveComponent *>(HitActor->PrimComponent);
		}
	}
	return nullptr;
}

// int32 UBuildingSystem::HitTestSurfaceByCursor(UObject *WorldContextObject)
// {
// 	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
// 	UGameViewportClient *GameViewportClient = MyWorld ? MyWorld->GetGameViewport() : nullptr;
// 	FSceneViewport *Viewport = GameViewportClient ? GameViewportClient->GetGameViewport() : nullptr;
// 	if (Viewport)
// 	{
// 		int32 MouseX = Viewport->GetMouseX();
// 		int32 MouseY = Viewport->GetMouseY();
// 
// 		GameViewportClient->DePr
// 
// 		HActor *HitActor = HitProxyCast<HActor>(Viewport->GetHitProxy(MouseX, MouseY));
// 		if (HitActor)
// 		{
// 			UPrimitiveComponent *Comp =  const_cast<UPrimitiveComponent *>(HitActor->PrimComponent);
// 
// 		}
// 	}
// 	return nullptr;
// }

bool UBuildingSystem::GetWallBorderLines(int32 WallID, TArray<FVector>&WallNodes, float &ZPos)
{
	bool bSuccess = false;
	if (Suite&&WallID != INVALID_OBJID)
	{
		UBuildingData *WallData = GetData(WallID);
		if (WallData)
		{
			ZPos = WallData->GetFloat("Height");
			kLine CenterLine;
			kLine LeftLine;
			kLine RightLine;
			bSuccess=Suite->GetWallBorderLines(WallID, CenterLine, LeftLine, RightLine);
			if (bSuccess)
			{
				WallNodes.Empty();
				WallNodes.Add(ToUE4Vector(CenterLine.start));
				WallNodes.Add(ToUE4Vector(LeftLine.start));
				WallNodes.Add(ToUE4Vector(LeftLine.end));
				WallNodes.Add(ToUE4Vector(CenterLine.end));
				WallNodes.Add(ToUE4Vector(RightLine.end));
				WallNodes.Add(ToUE4Vector(RightLine.start));
			}
		}
	}
	return bSuccess;
}

void UBuildingSystem::OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID SurfaceID)
{
	int32 ObjID = RawObj->GetID();
	FObjectInfo *ObjInfo = ObjMap.Find(ObjID);
	if (ObjInfo)
	{
		for (int32 i = 0; i < ObjInfo->Actorts.Num(); ++i)
		{
			ABuildingActor *Actor = Cast<ABuildingActor>(ObjInfo->Actorts[i]);
			UBuildingComponent *BuildingComp = Actor ? Actor->FindComponentByClass<UBuildingComponent>() : nullptr;
			if (BuildingComp)
			{
				BuildingComp->UpdateSurface(SectionIndex, SurfaceID);
			}
		}
	}
}
IValue* UBuildingSystem::GetProperty(ObjectID ID, const char *PropertyName)
{
	IValue *v = nullptr;
	if (Suite)
	{
		v=&Suite->GetProperty(ID, PropertyName);
	}
	return v;

}
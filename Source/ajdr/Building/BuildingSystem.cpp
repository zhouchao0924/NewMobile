
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
{
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

void UBuildingSystem::LoadFile(const FString &InFilename)
{
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
		}
	}
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
				SpawnActorByObject(MyWorld, ObjInfo);
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
					SpawnActorByObject(World, *ObjInfo);
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
		}
	}
}

ADRActor *UBuildingSystem::SpawnActorByObject(UWorld *World, FObjectInfo &ObjInfo)
{
	ADRActor *pActor = nullptr;
	IObject *Obj = ObjInfo.Data->GetRawObj();

	if (!pActor)
	{
		int nn = Obj->GetType();
		if (!nn==0)
		{
			int mm = Obj->GetType();
		}
		if (Obj->IsA(EPrimitive))
		{
			pActor = SpawnPrimitiveComponent(World, ObjInfo, Obj->GetType());
		}
		else if (Obj->IsA(EModelInstance))
		{
			pActor = SpawnModelComponent(World, ObjInfo);
		}
	}

	if (Visitor)
	{
		Visitor->OnCheckObjectVisible(ObjInfo);
	}
	return pActor;
}

ADRActor * UBuildingSystem::SpawnModelComponent(UWorld *MyWorld, FObjectInfo &ObjInfo)
{
	AModelFileActor *Actor = (AModelFileActor *)MyWorld->SpawnActor(AModelFileActor::StaticClass(), &FTransform::Identity);
	if (Actor)
	{
		Actor->Update(ObjInfo.Data);
		ObjInfo.Actorts.Add(Actor);
	}
	return Actor;
}

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

bool UBuildingSystem::IsFree(int32 ObjID)
{
	return Suite ? Suite->IsFree(ObjID) : false;
}

int32 UBuildingSystem::GetAllObjects(IObject** &ppOutObject, EObjectType InClass)
{
	if (Suite)
	{
		return Suite->GetAllObjects(ppOutObject, InClass);
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

int32 UBuildingSystem::AddModelToObject(int32 BaseObjID, const FString &ResID, const FVector &Location)
{
	if (Suite)
	{
		const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
		kVector3D Loc = ToBuildingPosition(Location);
		return Suite->AddModelToObject(BaseObjID, AnsiResID, Loc);
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
	ObjectID ConfigID = GetSuite()->GetConfig();
	GetSuite()->SetProperty(ConfigID, "WallMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->WallMaterial->GetPathName())));
	GetSuite()->SetProperty(ConfigID, "WallMaterialType", &GetValueFactory()->Create(Config->WallMaterialType));
	GetSuite()->SetProperty(ConfigID, "FloorMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->FloorMaterial->GetPathName())));
	GetSuite()->SetProperty(ConfigID, "FloorMaterialType", &GetValueFactory()->Create(Config->FloorMaterialType));
	GetSuite()->SetProperty(ConfigID, "CeilMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->WallMaterial->GetPathName())));
	GetSuite()->SetProperty(ConfigID, "CeilMaterialType", &GetValueFactory()->Create(Config->CeilMaterialType));
	GetSuite()->SetProperty(ConfigID, "bCeilVisible", &GetValueFactory()->Create(Config->bCeilVisible));
	GetSuite()->SetProperty(ConfigID, "Tolerance", &GetValueFactory()->Create(Config->Tolerance));
	//GetSuite()->SetProperty(ConfigID, "SkirtingCeil", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->SkirtingCeil)));
	//GetSuite()->SetProperty(ConfigID, "SkirtingCeilExt", &GetValueFactory()->Create(&ToBuildingPosition(Config->SkirtingCeilExt)));
	//GetSuite()->SetProperty(ConfigID, "SkirtingFloor", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->SkirtingFloor)));
	//GetSuite()->SetProperty(ConfigID, "SkirtingFloorExt", &GetValueFactory()->Create(&ToBuildingPosition(Config->SkirtingFloorExt)));
	GetSuite()->SetProperty(ConfigID, "DefaultDoor", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultDoor)));
	GetSuite()->SetProperty(ConfigID, "DefaultDoorFrame", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultDoorFrame)));
	GetSuite()->SetProperty(ConfigID, "DefaultWindow", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultWindow)));
}


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

void  UBuildingSystem::AddVirtualWall(ObjectID StartCorner, ObjectID EndCorner)
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
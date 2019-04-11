// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISuite.h"
#include "BuildingData.h"
#include "BuildingConfig.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "IBuildingSDK.h"
#include "BuildingSystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FBuildingObjectChangedDelegate, class UBuildingData *, Data);

USTRUCT(BlueprintType)
struct FObjectInfo
{
	GENERATED_BODY()
	
	FObjectInfo()
		:Data(NULL)
	{
	}
	
	FObjectInfo(UBuildingData *InData)
		:Data(InData)
	{
	}

	UPROPERTY()
	UBuildingData *Data;

	UPROPERTY()
	FBuildingObjectChangedDelegate Delegate;
	
	UPROPERTY()
	TArray<class ADRActor *>  Actorts;
};

class IBuildingSDK;

struct FSlateContext
{
	TSharedPtr<FSlateBrush>			WallBrush;
	FSlateResourceHandle			WallRenderingResourceHandle;
	TSharedPtr<FSlateBrush>			AreaBrush;
	FSlateResourceHandle			AreaRenderingResourceHandle;
};

class IBuildingVisitor
{
public:
	virtual void OnCheckObjectVisible(FObjectInfo &ObjInfo) = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuildingPropertyChangedHander, UBuildingData *, Data, const FString &, PropName);

class ADRActor;
class ABuildingActor;

UCLASS(BlueprintType)
class UBuildingSystem :public UObject, public ISuiteListener, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void LoadFile(const FString &InFilename);
	
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SaveFile(const FString &InFilename);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void ClearSuite();

	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	void AddToWorld(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	void RemoveFromWorld(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetRoomCeilVisible (bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	bool IsRoomCeilVisible();

	UFUNCTION(BlueprintCallable, Category = "Suite")
	UBuildingData *GetData(int32 ID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddCorner(const FVector2D &Location);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddModelToObject(int32 BaseObjID, const FString &ResID, const FVector &Location);
		
	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddModelToAnchor(int32 AnchorID, const FString &ResID, const FVector &Location);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void AddWall(int32 StartCorner, int32 EndCorner, float ThickLeft = 20.0f, float ThickRight = 20.0f, float Height = 280.0f);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddWindow(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddDoor(int32 WallID, const FVector2D &Location, float Width, float Height, float zPos = 0);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32  FindCloseWall(const FVector2D &Location, float Width, FVector2D &BestLoc,float Tolerance = -1.0f);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void Build();

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetChannelVisbile(EVisibleChannel Channel, bool bVisible);

	//UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 GetAllObjects(IObject** &ppOutObject, EObjectType InClass = EUnkownObject);

	void LoadingConfig(FBuildingConfig * Config);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void GetAllCornerActorLoction(TArray<FVector2D> &OutAllCornerActorLoction, int32 objectID);

	ObjectID IsHitCornner(const FVector2D &Location);

	ObjectID HitTest(const FVector2D &Location);

	bool Move(ObjectID objID, const FVector2D &DeltaMove);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 GetPolygon(int32 objID, TArray<FVector2D>& TPolygons, bool binner);

	int	HitTestMinMax(const FVector2D &Min, const FVector2D &Max, int32 *&pResults);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 GetRoomByLocation(const FVector2D &Loc);

	UFUNCTION(BlueprintCallable, Category = "Suite", meta=(WorldContext="WorldContextObject"))
	static UPrimitiveComponent *HitTestPrimitiveByCursor(UObject *WorldContextObject);

	bool GetWallVector(int32 WallID, FVector2D &P0, FVector2D &P1, FVector2D &Right);

	void  AddVirtualWall(ObjectID StartCorner, ObjectID EndCorner);

	int32 GetConnectWalls(int32 objID, int32 *&pConnectedWalls);

	bool GetWallBorderLines(int32 WallID, TArray<FVector>&WallNodes,float & ZPos);
	
	void OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID SurfaceID);

	IObject *GetObject(int32 ObjID);
	void DeleteObject(int32 ObjID);
	ISuite *GetSuite() { return Suite; }
	bool IsFree(int32 ObjID);
	void Tick(float DeltaTime);
	bool IsTickable() const { return true;}
	int32 Snap(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance);
	TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UBuildingSystem, STATGROUP_Tickables); }
	void OnPropertyChanged(UBuildingData *Data, const FString &PropName, const IValue &NewValue);
	void SetSelected(ObjectID ObjID, bool bSelected);
	static UBuildingSystem *LoadNewSuite(UObject *Outer, const FString &InFilename);
	static UBuildingSystem *CreateNewSuite(UObject *Outer, const FString &InFilename = TEXT(""));
	static IBuildingSDK  *GetBuildingSDK();
	static IValueFactory *GetValueFactory();
	UWorld *GetWorld() const override;
	void SetVisitor(IBuildingVisitor *Visitor);
	void UpdateChannelVisible();
protected:
	void LoadObjInfo();
	void OnAddObject(IObject *RawObj);
	void OnDeleteObject(IObject *RawObj);
	void OnUpdateObject(IObject *RawObj, unsigned int ChannelMask);
	void ClearObjInfo(FObjectInfo *ObjInfo);
	FObjectInfo *NewSuiteData(IObject *RawObj);
	ADRActor *SpawnActorByObject(UWorld *World, FObjectInfo &ObjInfo);
	int32 FindHostWorld(UWorld *World);
	ADRActor *SpawnPrimitiveComponent(UWorld *MyWorld, FObjectInfo &ObjInfo, int ObjectType);
	ADRActor *SpawnModelComponent(UWorld *MyWorld, FObjectInfo &ObjInfo);
public:
	UPROPERTY(BlueprintReadOnly, Category = "Suite")
	FString						Filename;
	
	UPROPERTY(BlueprintReadOnly, Category = "Suite")
	TMap<int32, FObjectInfo>	ObjMap;

	UPROPERTY(Transient, BlueprintAssignable, Category = "3D")
	FBuildingPropertyChangedHander EventPropertyChangedHandler;
	UPROPERTY(Transient, BlueprintReadWrite)
	UObject						*BuilldActorFactory;

	IBuildingVisitor			*Visitor;
	TArray<FWeakObjectPtr>		HostWorlds;
	ISuite						*Suite;
	FSlateContext				SlateContext;
	static	IBuildingSDK		*BuildingSDK;
};



#include "ajdr.h"
#include "DRDrawer.h"
#include "EditMode_Design2D.h"
#include "Camera/CameraMod_2D.h"
#include "DREditorViewportClient.h"
#include "Building/BuildingDrawer.h"
#include "Building/BuildingSystem.h"
#include "Building/BuildingActor.h"
#include "DRHitProxy.h"

UEditMode_Design2D::UEditMode_Design2D(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UEditMode_Design2D::OnEnter()
{
	Super::OnEnter();

	FDREditorViewportClient *ViewportClient = GetViewportClient();
	if (ViewportClient)
	{
		ViewportClient->SetCameraMod(ECameraMod::Camera2D);
		ViewportClient->SetBuildingActorVisibleOnly(false);
		ViewportClient->CameraMgr.SetViewType(false);
	}
}

void UEditMode_Design2D::OnCheckObjectVisible(FObjectInfo &ObjInfo)
{
	for (int32 i = 0; i < ObjInfo.Actorts.Num(); ++i)
	{
		ADRActor *pActor = ObjInfo.Actorts[i];
		if (pActor->IsA(ABuildingActor::StaticClass()))
		{
			pActor->SetActorHiddenInGame(true);
		}
		else
		{
			pActor->SetActorHiddenInGame(false);
		}
	}
}

void UEditMode_Design2D::Draw2D(FDrawer *Drawer)
{
	Super::Draw2D(Drawer);

	for (TMap<int32, FObjectInfo>::TIterator It(BuildingSystem->ObjMap); It; ++It)
	{
		UBuildingData *Data = It.Value().Data;
		IObject *pObj = Data? Data->GetRawObj() : nullptr;
		if (pObj)
		{
			if (pObj->IsA(EWall))
			{
				FBuildingDrawer::DrawWall(Drawer, pObj, false);
			}
			else if (pObj->IsA(EDoorHole))
			{
				FBuildingDrawer::DrawDoor(Drawer, pObj, false);
			}
			else if (pObj->IsA(EWindow))
			{
				FBuildingDrawer::DrawWindow(Drawer, pObj, false);
			}
		}
	}	
}




#include "ajdr.h"
#include "EditMode_Design3D.h"

UEditMode_Design3D::UEditMode_Design3D(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UEditMode_Design3D::OnEnter()
{
	Super::OnEnter();

	FDREditorViewportClient *ViewportClient = GetViewportClient();
	if (ViewportClient)
	{
		ViewportClient->SetCameraMod(ECameraMod::Fly);
		ViewportClient->SetBuildingActorVisibleOnly(true);
	}

	UBuildingSystem *BuildingSys = GetBuildingSystem();
	if (BuildingSys)
	{
		BuildingSys->Build();
	}
}

void UEditMode_Design3D::OnCheckObjectVisible(FObjectInfo &ObjInfo)
{
	for (int32 i = 0; i < ObjInfo.Actorts.Num(); ++i)
	{
		ADRActor *pActor = ObjInfo.Actorts[i];
		if (pActor->GetChannel() == EVisibleChannel::EBuildingTopChannel)
		{
			pActor->SetActorHiddenInGame(true);
		}
		else
		{
			pActor->SetActorHiddenInGame(false);
		}
	}
}




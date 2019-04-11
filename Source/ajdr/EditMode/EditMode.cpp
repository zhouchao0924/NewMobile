
#include "ajdr.h"
#include "DRGameMode.h"
#include "EditMode.h"
#include "DRMainFrame.h"

UEditMode::UEditMode(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, ModMgr(nullptr)
	, BuildingSystem(nullptr)
	, LastOperation(EOperationType::EOperation_None)
	, CurrentOperation(EOperationType::EOperation_None)
{
}

UWorld *UEditMode::GetWorld() const
{
	UObject *Outter = GetOuter();
	return Outter ? Outter->GetWorld() : nullptr;
}

UWorld *UEditMode::GetPreviewWorld()
{
	UWorld *PreviewWorld = nullptr;
	FDREditorViewportClient *ViewportClient = GetViewportClient();
	if (ViewportClient)
	{
		PreviewWorld = ViewportClient->GetPreviewWorld();
	}
	return PreviewWorld;
}

FDREditorViewportClient *UEditMode::GetViewportClient()
{
	FDREditorViewportClient *ViewportClient = nullptr;
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		ViewportClient = ADRGameMode::GetViewportClient(MyWorld);
	}
	return ViewportClient;
}

void UEditMode::OnEnter()
{
	if (DefaultOperation != EOperationType::EOperation_None)
	{
		ActiveOperation(DefaultOperation);
	}

	FDREditorViewportClient *ViewportClient = GetViewportClient();
	if (ViewportClient)
	{
		OnInputKeyDelegate = ViewportClient->OnInputKeyCallback.AddUObject(this, &UEditMode::InputKey);
	}

	UBuildingSystem *BuildingSys = GetBuildingSystem();
	if (BuildingSys)
	{
		BuildingSys->SetVisitor(this);
		BuildingSys->UpdateChannelVisible();
	}

	UDRMainFrame *MainFrame = ADRGameMode::GetMainFrame(this);
}

void UEditMode::OnLeave()
{
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if (ViewportClient && OnInputKeyDelegate.IsValid())
	{
		ViewportClient->OnInputKeyCallback.Remove(OnInputKeyDelegate);
		OnInputKeyDelegate.Reset();
	}

	ActiveOperation(EOperationType::EOperation_None);
}

void UEditMode::Draw2D(FDrawer *Drawer)
{
	UOperation *Op = GetOperation(CurrentOperation);
	if (Op)
	{
		Op->Draw2D(Drawer);
	}
}

void UEditMode::SetBuildingSystem(UBuildingSystem *InBuildingSystem)
{
	BuildingSystem = InBuildingSystem;
}

void UEditMode::InputKey(FViewport *Viewport, FKey Key, EInputEvent Event)
{
	if ( Key == EKeys::SpaceBar && 
		 Event==IE_Pressed &&
		 LastOperation != EOperationType::EOperation_None)
	{
		ActiveOperation(LastOperation);
	}
}

void UEditMode::ActiveOperation(EOperationType InType)
{
	if (CurrentOperation != InType)
	{
		UOperation *CurOp = GetOperation(CurrentOperation);
		if (CurOp)
		{
			CurOp->Deactive();
		}

		UOperation *FoundOp = GetOperation(InType);
		if (FoundOp)
		{
			FoundOp->Active();
		}

		CurrentOperation = InType;
	}
}

void UEditMode::CancleOperation()
{
	UOperation *CurOp = GetOperation(CurrentOperation);
	if (CurOp)
	{
		CurOp->Deactive();
		LastOperation = CurrentOperation;
		CurrentOperation = EOperationType::EOperation_None;
	}
}

UOperation *UEditMode::GetOperation(EOperationType InType)
{
	UOperation *FoundOp = nullptr;
	for (int32 i = 0; i < Operations.Num(); ++i)
	{
		UOperation *Op = Operations[i];
		if (Op && Op->GetType() == InType)
		{
			FoundOp = Op;
			break;
		}
	}
	return FoundOp;
}

void UEditMode::Awake()
{
	for (int32 i = 0; i < Operations.Num(); ++i)
	{
		UOperation *Op = Operations[i];
		if (Op!=nullptr)
		{
			Op->EditMode = this;
			Op->Awake();
		}
	}
}

void UEditMode::Sleep()
{
	for (int32 i = 0; i < Operations.Num(); ++i)
	{
		UOperation *Op = Operations[i];
		if (Op != nullptr)
		{
			Op->Sleep();
			Op->EditMode = nullptr;
		}
	}
}







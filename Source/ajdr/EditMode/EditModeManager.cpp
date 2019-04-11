
#include "ajdr.h"
#include "DRGameMode.h"
#include "EditModeManager.h"
#include "Design2D/EditMode_Design2D.h"
#include "Design3D/EditMode_Design3D.h"
#include "SceneManagement.h"

UEditModeManager::UEditModeManager(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	,CurrentMode(nullptr)
{
}

void UEditModeManager::BeginPlay()
{
	for (int32 i = 0; i < Modes.Num(); ++i)
	{
		UEditMode *EditMod = Modes[i];
		if (EditMod)
		{
			EditMod->ModMgr = this;
			EditMod->Awake();
		}
	}

	FDREditorViewportClient *ViewportClient = ADRGameMode::GetViewportClient(this);
	if (ViewportClient)
	{
		ViewportClient->OnDrawCallback.BindUObject(this, &UEditModeManager::Draw2D);
	}
}

void UEditModeManager::Draw2D(FDrawer *Drawer)
{
	if (CurrentMode)
	{
		CurrentMode->Draw2D(Drawer);
	}
}

void UEditModeManager::SetEditMode(EEditMode Mode)
{
	if (!CurrentMode || CurrentMode->GetMode() != Mode)
	{
		ModeStacks.Empty();
		PushMode(Mode);
	}
}

void UEditModeManager::SetCurrentModeOperation(EOperationType Operation)
{
	if (CurrentMode)
	{
		CurrentMode->ActiveOperation(Operation);
	}
}

void UEditModeManager::SetBuildingSystem(UBuildingSystem *InBuildingSystem)
{
	for (int32 i = 0; i < Modes.Num(); ++i)
	{
		UEditMode *EditMod = Modes[i];
		if (EditMod)
		{
			EditMod->SetBuildingSystem(InBuildingSystem);
		}
	}
}

void UEditModeManager::PushMode(EEditMode Mode)
{
	UEditMode *pMode = GetMode(Mode);
	if (pMode)
	{
		if (CurrentMode)
		{
			CurrentMode->OnLeave();
		}

		pMode->OnEnter();
		CurrentMode = pMode;
		ModeStacks.Add(Mode);
	}
}

void UEditModeManager::PopMode()
{
	if (ModeStacks.Num() > 1)
	{
		ModeStacks.Pop();

		EEditMode NewMode = ModeStacks.Last();
		UEditMode *pMode = GetMode(NewMode);
		check(pMode);
		
		if (CurrentMode)
		{
			CurrentMode->OnLeave();
		}

		pMode->OnEnter();
		CurrentMode = pMode;
	}
}

UEditMode *UEditModeManager::GetMode(EEditMode ModeType)
{
	UEditMode *FoundMode = nullptr;

	for (int32 i = 0; i < Modes.Num(); ++i)
	{
		UEditMode *Mode = Modes[i];
		if (Mode && Mode->GetMode()== ModeType)
		{
			FoundMode = Mode;
			break;
		}
	}
	return FoundMode;
}

UWorld *UEditModeManager::GetWorld() const
{
	UObject *Outter = GetOuter();
	return Outter ? Outter->GetWorld() : nullptr;
}


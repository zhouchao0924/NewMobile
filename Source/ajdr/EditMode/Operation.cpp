
#include "ajdr.h"
#include "Operation.h"
#include "EditMode.h"
#include "DRGameMode.h"

UOperation::UOperation(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, ViewportWidget(nullptr)
	, bNeedKeyInput(0)
	, bNeedAxisInput(0)
	, EditMode(nullptr)
{
}

UWorld *UOperation::GetWorld() const
{
	UObject *Outer = GetOuter();
	return Outer ? Outer->GetWorld() : nullptr;
}

FDREditorViewportClient *UOperation::GetViewportClient()
{
	if (ViewportWidget)
	{
		return ViewportWidget->GetViewportClient();
	}
	return nullptr;
}

UBuildingSystem *UOperation::GetBuildingSystem()
{
	if (EditMode)
	{
		return EditMode->GetBuildingSystem();
	}
	return nullptr;
}

void UOperation::Awake()
{
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		ViewportWidget = ADRGameMode::GetViewportClientWidget(this);
	}
}

void UOperation::Sleep()
{
	ViewportWidget = nullptr;
}

void UOperation::Active()
{
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if (ViewportClient)
	{
		if (bNeedKeyInput)
		{
			OnInputKeyDelegate = ViewportClient->OnInputKeyCallback.AddUObject(this, &UOperation::InputKey);
		}

		if (bNeedAxisInput)
		{
			OnInputAxisDelegate = ViewportClient->OnInputAxisCallback.AddUObject(this, &UOperation::InputAxis);
		}
	}
}

void UOperation::Deactive()
{
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if (ViewportClient)
	{
		if (OnInputKeyDelegate.IsValid())
		{
			ViewportClient->OnInputKeyCallback.Remove(OnInputKeyDelegate);
			OnInputKeyDelegate.Reset();
		}

		if (OnInputAxisDelegate.IsValid())
		{
			ViewportClient->OnInputAxisCallback.Remove(OnInputAxisDelegate);
			OnInputAxisDelegate.Reset();
		}
	}
}

void UOperation::EnableKeyInput(bool bInEnableKeyInput)
{
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if (ViewportClient)
	{
		if (bInEnableKeyInput != bNeedAxisInput)
		{
			bNeedKeyInput = bInEnableKeyInput;
			if (bNeedKeyInput)
			{
				OnInputKeyDelegate = ViewportClient->OnInputKeyCallback.AddUObject(this, &UOperation::InputKey);
			}
			else
			{
				if (OnInputKeyDelegate.IsValid())
				{
					ViewportClient->OnInputKeyCallback.Remove(OnInputKeyDelegate);
					OnInputKeyDelegate.Reset();
				}
			}
		}
	}
}

void UOperation::EnableAxisInput(bool bInEnableAxisInput)
{
	FDREditorViewportClient *ViewportClient = GetViewportClient();

	if (ViewportClient)
	{
		if (bInEnableAxisInput != bNeedAxisInput)
		{
			bNeedAxisInput = bInEnableAxisInput;
			if (bNeedAxisInput)
			{
				OnInputAxisDelegate = ViewportClient->OnInputAxisCallback.AddUObject(this, &UOperation::InputAxis);
			}
			else
			{
				if (OnInputAxisDelegate.IsValid())
				{
					ViewportClient->OnInputAxisCallback.Remove(OnInputAxisDelegate);
					OnInputAxisDelegate.Reset();
				}
			}
		}
	}
}






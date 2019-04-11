// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Viewport/DREditorViewportWidget.h"
#include "Operation.generated.h"

UENUM(BlueprintType)
enum class EOperationType : uint8
{
	EOperation_Select,
	EOperation_DrawWall,
	EOperation_DrawDoor,
	EOperation_DrawWindow,
	EOperation_None
};

UCLASS(BlueprintType, abstract, editinlinenew)
class UOperation : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Awake();
	virtual void Sleep();
	virtual void Active();
	virtual void Deactive();
	virtual EOperationType GetType() { return EOperationType::EOperation_None; }
	virtual void InputKey(FViewport *Viewport, FKey Key, EInputEvent Event) { }
	virtual void InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime) { }
	virtual void Draw2D(FDrawer *Drawer) {}

	UFUNCTION(BlueprintCallable)
	UBuildingSystem *GetBuildingSystem();

	virtual UWorld *GetWorld() const;
	FDREditorViewportClient *GetViewportClient();
	friend class UEditMode;
public:
	UFUNCTION(BlueprintCallable)
	void EnableKeyInput(bool bInEnableKeyInput);
	
	UFUNCTION(BlueprintCallable)
	void EnableAxisInput(bool bInEnableAxisInput);
protected:
	UPROPERTY(EditAnywhere, Category = "Operation")
	uint8					 bNeedKeyInput : 1;
	UPROPERTY(EditAnywhere, Category = "Operation")
	uint8					 bNeedAxisInput : 1;
	UPROPERTY(Transient, BlueprintReadOnly)
	UDREditorViewportWidget	 *ViewportWidget;
	UPROPERTY(Transient, BlueprintReadOnly)
	class UEditMode			*EditMode;
	FDelegateHandle			 OnInputKeyDelegate;
	FDelegateHandle			 OnInputAxisDelegate;
};



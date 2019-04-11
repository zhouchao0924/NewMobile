// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operation.h"
#include "Building/BuildingSystem.h"
#include "DREditorViewportClient.h"
#include "EditMode.generated.h"

UENUM(BlueprintType)
enum class EEditMode :uint8
{
	Design2D,
	Design3D,
	Resource,
	Unknown,
};

UCLASS(Blueprintable, abstract, editinlinenew)
class UEditMode : public UObject, public IBuildingVisitor
{
	GENERATED_UCLASS_BODY()
public:
	virtual EEditMode GetMode() { return EEditMode::Unknown; }
	virtual void OnEnter();
	virtual void OnLeave();
	virtual void Draw2D(FDrawer *Drawer);

	UFUNCTION(BlueprintCallable)
	UWorld *GetPreviewWorld();
	
	UFUNCTION(BlueprintCallable)
	void ActiveOperation(EOperationType InType);

	UFUNCTION(BlueprintCallable)
	void CancleOperation();

	UFUNCTION(BlueprintCallable)
	UOperation *GetOperation(EOperationType InType);

	void Awake();
	void Sleep();
	UWorld *GetWorld() const override;
	FDREditorViewportClient *GetViewportClient();
	void SetBuildingSystem(UBuildingSystem *InBuildingSystem);
	UBuildingSystem *GetBuildingSystem() { return BuildingSystem; }
	void InputKey(FViewport *Viewport, FKey Key, EInputEvent Event);
	void OnCheckObjectVisible(FObjectInfo &ObjInfo) {}
	friend class UEditModeManager;
protected:
	UPROPERTY(Transient)
	class UEditModeManager  *ModMgr;
	UPROPERTY(Transient)
	UBuildingSystem			*BuildingSystem;
	UPROPERTY(EditAnywhere, Instanced, Category="Mode")
	TArray<UOperation *>	Operations;
	UPROPERTY(EditAnywhere, Category = "Mode")
	EOperationType			DefaultOperation;
	UPROPERTY(Transient,	BlueprintReadOnly)
	EOperationType			CurrentOperation;
	UPROPERTY(Transient, BlueprintReadOnly)
	EOperationType			LastOperation;
	FDelegateHandle			OnInputKeyDelegate;
};



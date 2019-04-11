// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditMode.h"
#include "Operation.h"
#include "EditModeManager.generated.h"

UCLASS(Blueprintable, abstract)
class UEditModeManager :public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetEditMode(EEditMode Mode);

	UFUNCTION(BlueprintCallable)
	void SetCurrentModeOperation(EOperationType Operation);

	UFUNCTION(BlueprintPure)
	UEditMode *GetMode() { return CurrentMode; }

	UFUNCTION(BlueprintCallable)
	void PushMode(EEditMode Mode);

	UFUNCTION(BlueprintCallable)
	void PopMode();

	void BeginPlay();
	UWorld *GetWorld() const override;
	void SetBuildingSystem(UBuildingSystem *InBuildingSystem);
protected:
	UEditMode *GetMode(EEditMode Mode);
	void Draw2D(class FDrawer *Drawer);
protected:
	UPROPERTY(Transient)
	UEditMode *CurrentMode;
	
	UPROPERTY(Transient)
	TArray<EEditMode> ModeStacks;

	UPROPERTY(EditAnywhere, Instanced)
	TArray<UEditMode *> Modes;
};



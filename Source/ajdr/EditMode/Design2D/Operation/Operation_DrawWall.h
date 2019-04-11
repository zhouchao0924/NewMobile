// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Operation.h"
#include "Operation_DrawWall.generated.h"

USTRUCT(BlueprintType)
struct  FWallSnapConfig
{
	GENERATED_BODY();
	FWallSnapConfig();

	UPROPERTY(EditAnywhere)
	bool	bTerminal;

	UPROPERTY(EditAnywhere)
	bool	bCenter;

	UPROPERTY(EditAnywhere)
	bool	bDrawNormal;

	UPROPERTY(EditAnywhere)
	bool	bHorizontalOrVertical;

	UPROPERTY(EditAnywhere)
	bool	bAuxLine;

	UPROPERTY(EditAnywhere)
	bool	bDrawAllSnapPoint;

	UPROPERTY(EditAnywhere)
	float	Radius;
};

UCLASS(BlueprintType)
class UOperation_DrawWall : public UOperation
{
	GENERATED_UCLASS_BODY()
	
	struct FAuxAxis
	{
		FVector2D	Start;
		FVector2D	End;
		FVector2D	BestLoc;
	};

	struct FSnapLoc
	{
		FSnapLoc() { Reset(); }
		bool		bActive;
		int32		BestID;
		FVector2D	Loc;
		void Reset() { bActive = false; BestID = INDEX_NONE; }
	};

	struct FSnapWallHole
	{
		FSnapWallHole() { Reset(); }
		bool		bActive;
		int32		BestID;
		FVector2D	Loc;
		bool IsActive() { return bActive && BestID != INDEX_NONE; }
		void Reset() { bActive = false; BestID = INDEX_NONE; }
	};
public:
	void CaclWorldPosition();
	void DrawAuxAxis(FDrawer *Drawer);
	void DrawSnapLoc(FDrawer *Drawer);
	void Draw2D(FDrawer *Drawer) override;
	EOperationType GetType() override { return EOperationType::EOperation_DrawWall; }
	void InputKey(FViewport *Viewport, FKey Key, EInputEvent Event) override;
	void InputAxis(FViewport *Viewport, FKey Key, float Delta, float DeltaTime) override;
protected:
	void HandleWallSnap();
	void HandleDrawWall(FViewport *Viewport, FKey Key, EInputEvent Event);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Operation")
	FWallSnapConfig				Config;
	FVector2D					_CurrentPosition;
	FSnapLoc					_SnapPoint;
	FSnapWallHole				_SnapHole;
	TArray<FAuxAxis>			_AuxAxisArray;
	int32						LastPoint;
};



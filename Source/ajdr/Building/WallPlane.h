// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "UserWidget.h"
#include "WallPlane.generated.h"

UCLASS()
class AJDR_API AWallPlaneBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallPlaneBase();
	UFUNCTION(BlueprintCallable)
		void InitPlane(const TArray<FVector> _TArrayPos, float ZPos,int32 ObjectID=-1, bool bgeneratecollision = false);
	UFUNCTION(BlueprintImplementableEvent, Category = "BuildPlane")
	void Createplane();
	UFUNCTION(BlueprintCallable)
	void SetRuler(bool IsShow);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> TrianglesID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> VerListPos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> NormalsDir;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProceduralMeshComponent *Plane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
	class UWidgetComponent* LeftLabelWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Label"))
	class UWidgetComponent* RightLabelWidget;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 WallID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bGeneratecollision;
};

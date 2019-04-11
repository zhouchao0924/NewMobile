
#pragma once

#include "Components/SceneComponent.h"
#include "CurtainComponent.generated.h"

UENUM(BlueprintType)
enum class ECurtainPart :uint8
{
	ERomebarL,
	ERomebarR,
	ERomebarM,
	ERomerbarMax,
};

class UModelFile;
class UModelPartComponent;

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class UCurtainComponent :public USceneComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetLength(float InLength);
	
	UFUNCTION(BlueprintCallable)
	UModelFile *GetRombarModel(int32 index);
	
	UFUNCTION(BlueprintCallable)
	UModelFile *GetClothModel(int32 index);

	UFUNCTION(BlueprintCallable)
	void SetCurtainInfo(const FString &InResRomebar1, const FString &InResCloth1, const FString &InResRomebar2, const FString &InResCloth2, float InLength, float DistanceBetweenBar);

	UFUNCTION(BlueprintCallable)
	void SetCloth(int32 index, const FString &InResCloth);

	UFUNCTION(BlueprintCallable)
	void SetRomerbar(int32 index, const FString &InResRomerbar);

	UFUNCTION(BlueprintCallable)
	void SetCurtainLength(float InLength);

	UFUNCTION(BlueprintCallable)
	void SetDistanceBetweenBar(float InDistance);
protected:
	void CreateCloths();
	void CreateRomerbars();
	void CreateModels();
	void UpdateLayout();
	void DestroyCloth();
	void DestroyRomerbar();
	void DestroyComponent(bool bPromoteChildren = false) override;
	UModelPartComponent *AddPart(UModelFile *ModelFile, int32 iModel);
	void DestroyComponents(TArray<UModelPartComponent *> &Components);
	bool IsTwoBar();
	void AddClothComponents(UModelFile *Cloth, TArray<UModelPartComponent *> &LeftClothComponents, TArray<UModelPartComponent *> &RightClothComponents);
	void AddRomerbarComponents(UModelFile *Romerbar, TArray<UModelPartComponent *> &RomerBarComponents);
	void UpdateComponentsLayout(const FVector &TwoBarOffset, UModelFile *RomebarModel, UModelFile *ClothModel, TArray<UModelPartComponent *> &LeftClothComponents, TArray<UModelPartComponent *> &RightClothComponents, TArray<UModelPartComponent *> &RomerBarComponents);
protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Romebar1;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Cloth1;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Romebar2;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString							Cloth2;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							Length;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							RomerbarHeadLength;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							RomerbarBodyLength;
	UPROPERTY(Transient, BlueprintReadOnly)
	float							DistanceBetweenBar;
	UPROPERTY(Transient)
	TArray<UModelPartComponent *>	LeftClothComponents1;
	UPROPERTY(Transient)
	TArray<UModelPartComponent *>	RightClothComponents1;
	UPROPERTY(Transient)
	TArray<UModelPartComponent *>	LeftClothComponents2;
	UPROPERTY(Transient)
	TArray<UModelPartComponent *>	RightClothComponents2;
	UPROPERTY(Transient)
	TArray<UModelPartComponent *>	RomerBarComponents1;
	UPROPERTY(Transient)
	TArray<UModelPartComponent *>	RomerBarComponents2;
};



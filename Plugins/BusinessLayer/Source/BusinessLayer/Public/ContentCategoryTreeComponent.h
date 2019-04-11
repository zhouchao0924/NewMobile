// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ContentCategoryTreeComponent.generated.h"

USTRUCT(BlueprintType)
struct FContentCategoryTreeNode
{
	GENERATED_BODY()
	
	FContentCategoryTreeNode();
	
	UPROPERTY(BlueprintReadOnly)
	FName Name;

	UPROPERTY(BlueprintReadOnly)
	int32 ParentID;

	UPROPERTY(BlueprintReadOnly)
	int32 Level;

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> Childs;
};

USTRUCT(BlueprintType)
struct FContentNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32			Id;

	UPROPERTY(BlueprintReadOnly)
	int32			ModelId;

	UPROPERTY(BlueprintReadOnly)
	int32			SpuId;

	UPROPERTY(BlueprintReadOnly)
	int32			Skuid;

	UPROPERTY(BlueprintReadOnly)
	int32			ModelCode;

	UPROPERTY(BlueprintReadOnly)
	int32			PurchasePrice;

	UPROPERTY(BlueprintReadOnly)
	int32			AijiaPrice;

	UPROPERTY(BlueprintReadOnly)
	int32			MarketPrice;

	UPROPERTY(BlueprintReadOnly)
	int32			Length;

	UPROPERTY(BlueprintReadOnly)
	int32			Width;

	UPROPERTY(BlueprintReadOnly)
	int32			Height;

	UPROPERTY(BlueprintReadOnly)
	FString			ProductName;

	UPROPERTY(BlueprintReadOnly)
	FString			ImageUrl;

	UPROPERTY(BlueprintReadOnly)
	FString			LocalPath;

	UPROPERTY(BlueprintReadOnly)
	FString			MxUrl;

	UPROPERTY(BlueprintReadOnly)
	FString			MxResID;
};

USTRUCT(BlueprintType)
struct FPageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32			PageIndex;
	UPROPERTY(BlueprintReadOnly)
	int32			PageCount;
	UPROPERTY(BlueprintReadOnly)
	int32			FirstIndex;
	UPROPERTY(BlueprintReadOnly)
	int32			LastIndex;
	UPROPERTY(BlueprintReadOnly)
	int32			TotalCount;
};

UCLASS(BlueprintType)
class BUSINESSLAYER_API UContentCategoryTreeComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	int32 GetParentCategoryTreeNodeID(int32 ID);

	UFUNCTION(BlueprintCallable)
	int32 GetCategoryTreeNodeID(int32 ParentID, const FName &Name);


	UFUNCTION(BlueprintCallable)
	FContentCategoryTreeNode GetCategoryTreeNode(int32 ID);

	void OnUnregister() override;
	void UpdateFromJson(class UVaRestJsonObject *Json);
	void UpdateGoodsFromJson(class UVaRestJsonObject *Json);
	void UpdatePageInfoFromJson(class UVaRestJsonObject *Json);
protected:
	void CleanCategoories();
public:
	UPROPERTY(Transient, BlueprintReadOnly)
	int32								RootID;
	UPROPERTY(Transient, BlueprintReadOnly)
	FPageInfo							PageInfo;
	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<FContentNode>				Goods;
	TArray<FContentCategoryTreeNode *>	Categories;
	FContentCategoryTreeNode			NullCategory;
};



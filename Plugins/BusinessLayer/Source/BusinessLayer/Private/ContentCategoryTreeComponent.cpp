// Fill out your copyright notice in the Description page of Project Settings.

#include "ContentCategoryTreeComponent.h"
#include "VaRestJsonValue.h"
#include "VaRestJsonObject.h"

FContentCategoryTreeNode::FContentCategoryTreeNode()
	:ParentID(INDEX_NONE)
	,Level(INDEX_NONE)
{
}

//////////////////////////////////////////////////////////////////////////
UContentCategoryTreeComponent::UContentCategoryTreeComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, RootID(INDEX_NONE)
{
}

int32 UContentCategoryTreeComponent::GetCategoryTreeNodeID(int32 ParentID, const FName &Name)
{
	FContentCategoryTreeNode Category = GetCategoryTreeNode(ParentID);
	for (int32 i = 0; i < Category.Childs.Num(); ++i)
	{
		int32 ID = Category.Childs[i];
		if (Categories.IsValidIndex(ID) && Categories[ID]->Name == Name)
		{
			return ID;
		}
	}
	return INDEX_NONE;
}

FContentCategoryTreeNode UContentCategoryTreeComponent::GetCategoryTreeNode(int32 ID)
{
	if (Categories.IsValidIndex(ID))
	{
		return *Categories[ID];
	}
	return NullCategory;
}

int32 UContentCategoryTreeComponent::GetParentCategoryTreeNodeID(int32 ID)
{
	if (Categories.IsValidIndex(ID))
	{
		return Categories[ID]->ParentID;
	}
	return INDEX_NONE;
}

void UContentCategoryTreeComponent::OnUnregister()
{
	Super::OnUnregister();
	CleanCategoories();
}

void UContentCategoryTreeComponent::CleanCategoories()
{
	for (int32 i = 0; i < Categories.Num(); ++i)
	{
		FContentCategoryTreeNode *pCategory = Categories[i];
		if (pCategory)
		{
			delete pCategory;
		}
	}
	Categories.Empty();
}

void UContentCategoryTreeComponent::UpdateFromJson(class UVaRestJsonObject *Json)
{
	if (Json != nullptr)
	{
		CleanCategoories();

		RootID = Categories.Add(new FContentCategoryTreeNode());

		struct FStackNode
		{
			FStackNode(int32 InParentID, int32 InID, UVaRestJsonObject *InJson)
				: ID(InID)
				, Json(InJson)
				, ParentID(InParentID)
			{
			}
			int32 ID;
			int32 ParentID;
			UVaRestJsonObject *Json;
		};

		TArray<FStackNode> Stacks;
		Stacks.Add(FStackNode(INDEX_NONE, RootID, Json));

		while (Stacks.Num()>0)
		{
			FStackNode ThisNode = Stacks.Pop();
			UVaRestJsonObject *Json = ThisNode.Json;
			FContentCategoryTreeNode *ThisCategoryNode = Categories[ThisNode.ID];

			UVaRestJsonValue *JsonValue = Json->GetField(TEXT("categoryDto"));
			if (JsonValue)
			{
				UVaRestJsonObject *JsonObj = JsonValue->AsObject();
				if (JsonObj)
				{
					FString Name = JsonObj->GetStringField(TEXT("name"));
					ThisCategoryNode->Name = *Name;
					ThisCategoryNode->ParentID = ThisNode.ParentID;
					ThisCategoryNode->Level = FMath::RoundToInt(JsonObj->GetNumberField(TEXT("level")));
				}
			}

			TArray<UVaRestJsonObject *> JsonArray = Json->GetObjectArrayField(TEXT("categoryTreeNodeDtos"));

			for (int32 i = 0; i < JsonArray.Num(); ++i)
			{
				UVaRestJsonObject *ChildJson = JsonArray[i];
				if (ChildJson)
				{
					int32 ID = Categories.Add(new FContentCategoryTreeNode());
					ThisCategoryNode->Childs.Add(ID);
					Stacks.Add(FStackNode(ThisNode.ID, ID, ChildJson));
				}
			}
		}
	}
}

void UContentCategoryTreeComponent::UpdatePageInfoFromJson(class UVaRestJsonObject *Json)
{
	PageInfo.FirstIndex = FMath::RoundToInt(Json->GetNumberField(TEXT("first")));
	PageInfo.LastIndex = FMath::RoundToInt(Json->GetNumberField(TEXT("last")));
	PageInfo.PageIndex = FMath::RoundToInt(Json->GetNumberField(TEXT("pageNo")));
	PageInfo.PageCount = FMath::RoundToInt(Json->GetNumberField(TEXT("pageSize")));
	PageInfo.TotalCount = FMath::RoundToInt(Json->GetNumberField(TEXT("count")));
}

void UContentCategoryTreeComponent::UpdateGoodsFromJson(class UVaRestJsonObject *Json)
{
	UpdatePageInfoFromJson(Json);

	TArray<UVaRestJsonObject *> JsonGoods = Json->GetObjectArrayField(TEXT("list"));
	int32 NumGoods = JsonGoods.Num();
	Goods.SetNum(NumGoods);

	for (int32 i = 0; i < NumGoods; ++i)
	{
		UVaRestJsonObject *JsonGood = JsonGoods[i];
		check(JsonGood);

		FContentNode &ItemGood = Goods[i];
		ItemGood.Id = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("id")));
		ItemGood.ModelId = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("modelId")));
		ItemGood.SpuId = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("spuId")));
		ItemGood.Skuid = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("skuId")));
		ItemGood.ModelCode = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("modelCode")));

		ItemGood.PurchasePrice = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("purchasePrice")));
		ItemGood.AijiaPrice = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("aijiaPrice")));
		ItemGood.MarketPrice = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("marketPrice")));
		ItemGood.Length = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("length")));
		ItemGood.Width = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("width")));
		ItemGood.Height = FMath::RoundToInt(JsonGood->GetNumberField(TEXT("height")));
		ItemGood.ProductName = JsonGood->GetStringField(TEXT("productName"));
		ItemGood.ImageUrl = JsonGood->GetStringField(TEXT("image"));
		ItemGood.MxUrl = JsonGood->GetStringField(TEXT("mxUrl"));
		ItemGood.MxResID = JsonGood->GetStringField(TEXT("mxFileMD5"));
	}
}



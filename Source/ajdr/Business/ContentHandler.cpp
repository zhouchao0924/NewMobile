// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "DRGameMode.h"
#include "VaRestJsonValue.h"
#include "VaRestJsonObject.h"

void ADRGameMode::CategoryHandler(UVaRestJsonObject *ResponseObj)
{
	bool bSuccess = ResponseObj->GetBoolField(TEXT("success"));
	UVaRestJsonValue *JsonValue = ResponseObj->GetField(TEXT("data"));

	if (bSuccess && JsonValue && ContentCategoryTree)
	{
		UVaRestJsonObject *JsonObj = JsonValue->AsObject();
		if (JsonObj)
		{
			ContentCategoryTree->UpdateFromJson(JsonObj);
		}
	}
}

void ADRGameMode::GoodsQueryByCondHandler(UVaRestJsonObject *ResponseObj)
{
	bool bSuccess = ResponseObj->GetBoolField(TEXT("success"));
	UVaRestJsonValue *JsonValue = ResponseObj->GetField(TEXT("data"));

	if (bSuccess && JsonValue && ContentCategoryTree)
	{
		UVaRestJsonObject *JsonObj = JsonValue->AsObject();
		if (JsonObj)
		{
			ContentCategoryTree->UpdateGoodsFromJson(JsonObj);
		}
	}
}

void ADRGameMode::GoodsPageInfoQueryHandler(UVaRestJsonObject *ResponseObj)
{
	bool bSuccess = ResponseObj->GetBoolField(TEXT("success"));
	UVaRestJsonValue *JsonValue = ResponseObj->GetField(TEXT("data"));

	if (bSuccess && JsonValue && ContentCategoryTree)
	{
		UVaRestJsonObject *JsonObj = JsonValue->AsObject();
		if (JsonObj)
		{
			ContentCategoryTree->UpdatePageInfoFromJson(JsonObj);
		}
	}
}



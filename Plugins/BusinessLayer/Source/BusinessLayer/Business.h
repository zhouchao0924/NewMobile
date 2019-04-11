// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Business.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBusinessHandler, UVaRestRequestJSON *, JSONRequest);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnBusinessReturn, bool, bSuccess, UVaRestRequestJSON *, JSONRequest);

class UVaRestRequestJSON;

USTRUCT(BlueprintType)
struct FBusinessHandle
{
	GENERATED_BODY()
	UPROPERTY(Transient, BlueprintReadWrite)
	FOnBusinessReturn	Response;
};

USTRUCT(BlueprintType)
struct FURLConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FString LoginURL;

	UPROPERTY(EditAnywhere)
	FString QueryCategoryURL;
	
	UPROPERTY(EditAnywhere)
	FString QueryGoodByConditionURL;
};

USTRUCT(BlueprintType)
struct FBusinessCall
{
	GENERATED_BODY()
	UPROPERTY(Transient, BlueprintReadWrite)
	FOnBusinessReturn	Response;
	UPROPERTY(Transient, BlueprintReadWrite)
	FString				URL;
	FDelegateHandle		RequestCompleteHandle;
	FDelegateHandle		RequestFailedHandle;
};

UENUM(BlueprintType)
enum class ERequestID :uint8
{
	ELoginID,
	EQuerySeries,
	EQueryGoodsByCond,
	EQueryGoodsPageInfo,
};

UCLASS(BlueprintType)
class BUSINESSLAYER_API UBusinessComponent :public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Login(const FString &UserName, const FString &Password, const FBusinessHandle &BusinessHandle, int32 TryCount = 1, int32 Milsec = 5000);

	UFUNCTION(BlueprintCallable)
	void Logout(const FBusinessHandle &BusinessHandle);

	UFUNCTION(BlueprintCallable)
	void QueryCategories(const FBusinessHandle &BusinessHandle);

	UFUNCTION(BlueprintCallable) 	/*ComplexFilter : skuId, modelCode, modelName, modelId, spuId  | default url: model/queryModelByCondition*/
	void QueryGoodsPageInfo(int32 pageSize, const FBusinessHandle &BusinessHandle, int32 TryCount = 1, int32 Milsec = 5000);

	UFUNCTION(BlueprintCallable) 	/*ComplexFilter : skuId, modelCode, modelName, modelId, spuId  | default url: model/queryModelByCondition*/
	void QueryGoodsByCondition(int32 typeOneId, int32 typeTwoId, int32 typeThreeId, int32 categoryId, const FString &brandIdOne, const FString &brandIdTwo, const FString & seriesId, const FString &complexFilter, const FString &complexFilterValue, int32 pageNo, int32 pageSize, const FBusinessHandle &BusinessHandle, int32 TryCount = 1, int32 Milsec = 5000);
public:
	void SetToken(const FString &InToken);
protected: 
	friend class UVaRestJsonObject;
	friend class UVaRestRequestJSON;
	void Cancel(UVaRestRequestJSON *Request);
	void OnRequestDone(UVaRestRequestJSON *Request);
	void CallJsonURL(ERequestID ID, UVaRestJsonObject *Json, const FBusinessHandle &BusinessHandle, const FString &SubURL, int32 TryCount = 1, int32 Milsec = 5000);
	static void OnRequestCompleted(UVaRestRequestJSON* Request);
	static void OnRequestFailed(UVaRestRequestJSON *Request);
	UVaRestRequestJSON *GetJSONRequest();
	static FBusinessCall *GetBusinessCall(UVaRestRequestJSON *Request);
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString										  RootURL;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FURLConfig									  URLConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32										  DefaultTryCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32										  DefaultTimeout;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString										  Token;
	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<UVaRestRequestJSON *, FBusinessCall>	  RequestMap;
	UPROPERTY(Transient)
	TArray<UVaRestRequestJSON *>				  Requests;
	UPROPERTY(BlueprintAssignable)
	FOnBusinessHandler							  OnBusinessHandler;
};



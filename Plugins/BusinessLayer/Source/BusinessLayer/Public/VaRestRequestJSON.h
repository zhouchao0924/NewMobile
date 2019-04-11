// Copyright 2014 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "Engine/LatentActionManager.h"
#include "Interfaces/IHttpRequest.h"
#include "HttpModule.h"
#include "VaRestTypes.h"
#include "VaRestRequestJSON.generated.h"

/** Generate a delegates for callback events */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestComplete, class UVaRestRequestJSON*, Request);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestFail, class UVaRestRequestJSON*, Request);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaticRequestComplete, class UVaRestRequestJSON*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaticRequestFail, class UVaRestRequestJSON*);

UCLASS(BlueprintType)
class BUSINESSLAYER_API UVaRestRequestJSON : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Request (Empty)", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VaRest|Request")
	static UVaRestRequestJSON* ConstructRequest(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Request", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VaRest|Request")
	static UVaRestRequestJSON* ConstructRequestExt(UObject* WorldContextObject, ERequestVerb Verb, ERequestContentType ContentType);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetVerb(ERequestVerb Verb);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetCustomVerb(FString Verb);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetContentType(ERequestContentType ContentType);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetBinaryContentType(const FString &ContentType);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetBinaryRequestContent(const TArray<uint8> &Content);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetStringRequestContent(const FString &Content);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetHeader(const FString& HeaderName, const FString& HeaderValue);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Utility")
	void ResetData();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Response")
	void Cancel();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	UVaRestJsonObject* GetRequestObject();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	void SetRequestObject(UVaRestJsonObject* JsonObject);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Response")
	UVaRestJsonObject* GetResponseObject();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Response")
	void SetResponseObject(UVaRestJsonObject* JsonObject);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	FString GetURL();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	ERequestStatus GetStatus();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Response")
	int32 GetResponseCode();

	UFUNCTION(BlueprintCallable, Category = "VaRest|Response")
	FString GetResponseHeader(const FString HeaderName);
	
	UFUNCTION(BlueprintCallable, Category = "VaRest|Response")
	TArray<FString> GetAllResponseHeaders();
public:
	UWorld *GetWorld() const override;
	UFUNCTION(BlueprintCallable, Category = "VaRest|Request")
	virtual void ProcessURL(const FString& Url = TEXT("http://alyamkin.com"));
	void ResetRequestData();
	void ResetResponseData();
	void ProcessRequest();
	UBusinessComponent *GetBusiness();
	void Clean();
private:
	void OnRequestTimeout();
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
public:
	UPROPERTY(BlueprintAssignable, Category = "VaRest|Event")
	FOnRequestComplete			OnRequestComplete;

	UPROPERTY(BlueprintAssignable, Category = "VaRest|Event")
	FOnRequestFail				OnRequestFail;

	FOnStaticRequestComplete	OnStaticRequestComplete;
	FOnStaticRequestFail		OnStaticRequestFail;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VaRest|Response")
	FString						ResponseContent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VaRest|Response")
	bool						bIsValidJsonResponse;
	int32						MaxTryCount;
	int32						Timeout;
	int32						RequestID;
protected:
	UPROPERTY()
	UVaRestJsonObject*			RequestJsonObj;
	UPROPERTY()
	UVaRestJsonObject*			ResponseJsonObj;
	TArray<uint8>				RequestBytes;
	FString						BinaryContentType;
	FString						StringRequestContent;	
	ERequestVerb				RequestVerb;
	ERequestContentType			RequestContentType;
	TMap<FString, FString>		RequestHeaders;
	TMap<FString, FString>		ResponseHeaders;
	int32						ResponseCode;
	FString						CustomVerb;
	TSharedPtr<IHttpRequest>	HttpRequest;
	int32						TryCount;
	FTimerHandle				TimeoutHandle;
};

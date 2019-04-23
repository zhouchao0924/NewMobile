
#include "Business.h"
#include "VaRestRequestJSON.h"
#include "VaRestJsonObject.h"

UBusinessComponent::UBusinessComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultTryCount(3)
	, DefaultTimeout(5000)
{
}

void UBusinessComponent::Login(const FString &UserName, const FString &Password, const FBusinessHandle &BusinessHandle, int32 TryCount/*= 5*/, int32 Milsec /*= 5000.0f*/)
{
	FString LoginURL;
	GConfig->GetString(TEXT("Business"), TEXT("LoginURL"), LoginURL, GGameIni);
	FString BaseHost;
	GConfig->GetString(TEXT("Business"), TEXT("BaseHost"), BaseHost, GGameIni);
	FString CallLoginURL = BaseHost.Append(LoginURL);

	UVaRestJsonObject *Json = NewObject<UVaRestJsonObject>(this);
	Json->SetStringField(TEXT("account"), "18512529274");
	Json->SetStringField(TEXT("password"), "Zqj85766389");
	Json->SetNumberField(TEXT("systemId"), 4);

	CallJsonURL(ERequestID::ELoginID, Json, BusinessHandle, CallLoginURL, TryCount, Milsec);
}

void UBusinessComponent::QuerySolutions(const FBusinessHandle &BusinessHandle)
{
	FString QuerySolutionURL;
	GConfig->GetString(TEXT("Business"), TEXT("QuerySolutionURL"), QuerySolutionURL, GGameIni);
	FString BaseHost;
	GConfig->GetString(TEXT("Business"), TEXT("BaseHost"), BaseHost, GGameIni);
	FString CallQuerySolutionURL = BaseHost.Append(QuerySolutionURL);

	UVaRestJsonObject *Json = NewObject<UVaRestJsonObject>(this);
	Json->SetNumberField(TEXT("pageNo"), 1);
	Json->SetNumberField(TEXT("pageSize"), 10);
	Json->SetNumberField(TEXT("searchUserId"), 42707);
	Json->SetNumberField(TEXT("solutionId"), 14403);
	Json->SetNumberField(TEXT("type"), 1);


	CallJsonURL(ERequestID::EQuerySolution, Json, BusinessHandle, CallQuerySolutionURL, DefaultTryCount, DefaultTimeout);
}

void UBusinessComponent::QueryReplace(const FBusinessHandle &BusinessHandle,const TArray<float> &skuids)
{
	FString QueryReplaceURL;
	GConfig->GetString(TEXT("Business"), TEXT("QueryReplaceURL"), QueryReplaceURL, GGameIni);
	FString BaseHost;
	GConfig->GetString(TEXT("Business"), TEXT("BaseHost"), BaseHost, GGameIni);
	FString CallQueryReplaceURL = BaseHost.Append(QueryReplaceURL);

	UVaRestJsonObject *Json = NewObject<UVaRestJsonObject>(this);
	Json->SetNumberArrayField(TEXT("skuIds"), skuids);
	
	CallJsonURL(ERequestID::EQuerySolution, Json, BusinessHandle, CallQueryReplaceURL, DefaultTryCount, DefaultTimeout);
}

void UBusinessComponent::QueryCategories(const FBusinessHandle &BusinessHandle)
{
	UVaRestJsonObject *Json = NewObject<UVaRestJsonObject>(this);
	CallJsonURL(ERequestID::EQuerySeries, Json, BusinessHandle, URLConfig.QueryCategoryURL, DefaultTryCount, DefaultTimeout);
}

void UBusinessComponent::QueryGoodsPageInfo(int32 pageSize, const FBusinessHandle &BusinessHandle, int32 TryCount /*= 1*/, int32 Milsec /*= 5000*/)
{
	UVaRestJsonObject *Json = NewObject<UVaRestJsonObject>(this);
	if (Json && pageSize>0)
	{
		Json->SetNumberField(TEXT("pageNo"), 1);
		Json->SetNumberField(TEXT("pageSize"), pageSize);
		CallJsonURL(ERequestID::EQueryGoodsPageInfo, Json, BusinessHandle, URLConfig.QueryGoodByConditionURL, TryCount, Milsec);
	}
}

void UBusinessComponent::QueryGoodsByCondition(int32 typeOneId, int32 typeTwoId, int32 typeThreeId, int32 categoryId, const FString &brandIdOne, const FString &brandIdTwo, const FString & seriesId, const FString &complexFilter, const FString &complexFilterValue, int32 pageNo, int32 pageSize, const FBusinessHandle &BusinessHandle, int32 TryCount /*= 1*/, int32 Milsec/* = 5000*/)
{
	UVaRestJsonObject *Json = NewObject<UVaRestJsonObject>(this);
	if (Json)
	{
		if (typeOneId > 0)
		{
			Json->SetNumberField(TEXT("typeOneId"), typeOneId);
		}
		
		if (typeTwoId > 0)
		{
			Json->SetNumberField(TEXT("typeTwoId"), typeTwoId);
		}
		
		if (typeThreeId > 0)
		{
			Json->SetNumberField(TEXT("typeThreeId"), typeThreeId);
		}
		
		if (categoryId > 0)
		{
			Json->SetNumberField(TEXT("categoryId"), categoryId);
		}

		if (brandIdOne.Len() > 0)
		{
			Json->SetStringField(TEXT("brandIdOne"), brandIdOne);
		}
	
		if (brandIdTwo.Len() > 0)
		{
			Json->SetStringField(TEXT("brandIdTwo"), brandIdTwo);
		}

		if (seriesId.Len() > 0)
		{
			Json->SetStringField(TEXT("seriesId"), seriesId);
		}

		if (complexFilter.Len() > 0 && complexFilterValue.Len() > 0)
		{
			Json->SetStringField(*complexFilter, complexFilterValue);
		}

		Json->SetNumberField(TEXT("pageNo"), pageNo);
		Json->SetNumberField(TEXT("pageSize"), pageSize);
	}

	CallJsonURL(ERequestID::EQueryGoodsByCond, Json, BusinessHandle, URLConfig.QueryGoodByConditionURL, TryCount, Milsec);
}

void UBusinessComponent::Logout(const FBusinessHandle &BusinessHandle)
{
}

void UBusinessComponent::SetToken(const FString &InToken)
{
	Token = InToken;
}

void UBusinessComponent::CallJsonURL(ERequestID ID, UVaRestJsonObject *Json, const FBusinessHandle &BusinessHandle, const FString &SubURL, int32 TryCount, int32 Milsec)
{
	FString URL = SubURL;

	UVaRestRequestJSON* Request = GetJSONRequest();

	Request->SetVerb(ERequestVerb::POST);
	Request->SetContentType(ERequestContentType::json);
	Request->SetRequestObject(Json);
	Request->SetHeader(TEXT("Authorization"), Token);
	Request->Timeout = Milsec;
	Request->MaxTryCount = TryCount;
	Request->RequestID = (int32)ID;

	FBusinessCall BusinessResponse;
	BusinessResponse.URL = URL;
	BusinessResponse.Response = BusinessHandle.Response;
	BusinessResponse.RequestCompleteHandle = Request->OnStaticRequestComplete.AddStatic(&UBusinessComponent::OnRequestCompleted);
	BusinessResponse.RequestFailedHandle = Request->OnStaticRequestFail.AddStatic(&UBusinessComponent::OnRequestFailed);

	RequestMap.Add(Request, BusinessResponse);

	Request->ResetResponseData();
	Request->ProcessURL(URL);
}

UVaRestRequestJSON *UBusinessComponent::GetJSONRequest()
{
	UVaRestRequestJSON *NewRequest = nullptr;
	if (Requests.Num() > 0)
	{
		NewRequest = Requests.Last();
		NewRequest->ResetData();
	}
	else
	{
		NewRequest = NewObject<UVaRestRequestJSON>(this);
	}
	return NewRequest;
}

FBusinessCall *UBusinessComponent::GetBusinessCall(UVaRestRequestJSON *Request)
{
	UBusinessComponent *MyBusiness = Request->GetBusiness();
	return MyBusiness? MyBusiness->RequestMap.Find(Request) : nullptr;
}

void UBusinessComponent::OnRequestCompleted(UVaRestRequestJSON* Request)
{
	FBusinessCall *pBusinessCall = GetBusinessCall(Request);
	if (pBusinessCall)
	{
		UBusinessComponent *MyBusiness = Request->GetBusiness();
		if (MyBusiness)
		{
			MyBusiness->OnRequestDone(Request);
			MyBusiness->OnBusinessHandler.Broadcast(Request);
		}
		pBusinessCall->Response.ExecuteIfBound(true, Request);
	}
}

void UBusinessComponent::OnRequestFailed(UVaRestRequestJSON *Request)
{
	FBusinessCall *pBusinessCall = GetBusinessCall(Request);
	if (pBusinessCall)
	{
		UBusinessComponent *MyBusiness = Request->GetBusiness();
		if (MyBusiness)
		{
			MyBusiness->OnRequestDone(Request);
		}
		pBusinessCall->Response.ExecuteIfBound(false, Request);
	}
}

void UBusinessComponent::Cancel(UVaRestRequestJSON *Request)
{
	OnRequestDone(Request);
}

void UBusinessComponent::OnRequestDone(UVaRestRequestJSON *Request)
{
	if (Request)
	{
		FBusinessCall *pBusinessCall = GetBusinessCall(Request);
		if (pBusinessCall)
		{
			Request->OnStaticRequestComplete.Remove(pBusinessCall->RequestCompleteHandle);
			Request->OnStaticRequestFail.Remove(pBusinessCall->RequestFailedHandle);
		}
		RequestMap.Remove(Request);
		Requests.Add(Request);
		Request->Clean();
	}
}



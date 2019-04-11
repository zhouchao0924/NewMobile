// Copyright 2014 Vladimir Alyamkin. All Rights Reserved.

#include "VaRestRequestJSON.h"
#include "CoreMisc.h"
#include "Business.h"
#include "BusinessLayer.h"
#include "VaRestLibrary.h"
#include "VaRestJsonObject.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UVaRestRequestJSON::UVaRestRequestJSON(const class FObjectInitializer& PCIP)
	: Super(PCIP)
	, BinaryContentType(TEXT("application/octet-stream"))
	, TryCount(0)
	, MaxTryCount(1)
	, Timeout(5000)
{
	RequestVerb = ERequestVerb::GET;
	RequestContentType = ERequestContentType::x_www_form_urlencoded_url;
	ResetData();
}

UVaRestRequestJSON* UVaRestRequestJSON::ConstructRequest(UObject* WorldContextObject)
{
	return NewObject<UVaRestRequestJSON>(WorldContextObject);
}

UVaRestRequestJSON* UVaRestRequestJSON::ConstructRequestExt( UObject* WorldContextObject,  ERequestVerb Verb,  ERequestContentType ContentType)
{
	UVaRestRequestJSON* Request = ConstructRequest(WorldContextObject);
	Request->SetVerb(Verb);
	Request->SetContentType(ContentType);
	return Request;
}

void UVaRestRequestJSON::SetVerb(ERequestVerb Verb)
{
	RequestVerb = Verb;
}

void UVaRestRequestJSON::SetCustomVerb(FString Verb)
{
	CustomVerb = Verb;
}

void UVaRestRequestJSON::SetContentType(ERequestContentType ContentType)
{
	RequestContentType = ContentType;
}

void UVaRestRequestJSON::SetBinaryContentType(const FString& ContentType)
{
	BinaryContentType = ContentType;
}

void UVaRestRequestJSON::SetBinaryRequestContent(const TArray<uint8>& Bytes)
{
	RequestBytes = Bytes;
}

void UVaRestRequestJSON::SetStringRequestContent(const FString& Content)
{
	StringRequestContent = Content;
}

void UVaRestRequestJSON::SetHeader(const FString& HeaderName, const FString& HeaderValue)
{
	RequestHeaders.Add(HeaderName, HeaderValue);
}

void UVaRestRequestJSON::ResetData()
{
	ResetRequestData();
	ResetResponseData();
}

void UVaRestRequestJSON::ResetRequestData()
{
	if (RequestJsonObj != nullptr)
	{
		RequestJsonObj->Reset();
	}
	else
	{
		RequestJsonObj = NewObject<UVaRestJsonObject>();
	}

	RequestBytes.Empty();
	StringRequestContent.Empty();
}

void UVaRestRequestJSON::ResetResponseData()
{
	if (ResponseJsonObj != nullptr)
	{
		ResponseJsonObj->Reset();
	}
	else
	{
		ResponseJsonObj = NewObject<UVaRestJsonObject>();
	}

	ResponseHeaders.Empty();
	ResponseCode = -1;

	bIsValidJsonResponse = false;
}

UBusinessComponent *UVaRestRequestJSON::GetBusiness()
{
	return Cast<UBusinessComponent>(GetOuter());
}

void UVaRestRequestJSON::Cancel()
{
	UBusinessComponent *MyBusiness = GetBusiness();
	if (MyBusiness)
	{
		MyBusiness->Cancel(this);
	}
}

void UVaRestRequestJSON::Clean()
{
	if (HttpRequest.IsValid())
	{
		HttpRequest->CancelRequest();
		HttpRequest.Reset();
	}

	if (TimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle);
		TimeoutHandle.Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////
UVaRestJsonObject* UVaRestRequestJSON::GetRequestObject()
{
	return RequestJsonObj;
}

void UVaRestRequestJSON::SetRequestObject(UVaRestJsonObject* JsonObject)
{
	RequestJsonObj = JsonObject;
}

UVaRestJsonObject* UVaRestRequestJSON::GetResponseObject()
{
	return ResponseJsonObj;
}

void UVaRestRequestJSON::SetResponseObject(UVaRestJsonObject* JsonObject)
{
	ResponseJsonObj = JsonObject;
}

FString UVaRestRequestJSON::GetURL()
{
	return HttpRequest->GetURL();
}

ERequestStatus UVaRestRequestJSON::GetStatus()
{
	return ERequestStatus((uint8)HttpRequest->GetStatus());
}

int32 UVaRestRequestJSON::GetResponseCode()
{
	return ResponseCode;
}

FString UVaRestRequestJSON::GetResponseHeader(const FString HeaderName)
{
	FString Result;

	FString* Header = ResponseHeaders.Find(HeaderName);
	if (Header != nullptr)
	{
		Result = *Header;
	}

	return Result;
}

TArray<FString> UVaRestRequestJSON::GetAllResponseHeaders()
{
	TArray<FString> Result;
	for (TMap<FString, FString>::TConstIterator It(ResponseHeaders); It; ++It)
	{
		Result.Add(It.Key() + TEXT(": ") + It.Value());
	}
	return Result;
}

//////////////////////////////////////////////////////////////////////////
void UVaRestRequestJSON::ProcessURL(const FString& Url)
{
	ResetResponseData();

	FString TrimmedUrl = Url;
	TrimmedUrl.TrimStart();
	TrimmedUrl.TrimEnd();

	if (!HttpRequest.IsValid())
	{
		HttpRequest = FHttpModule::Get().CreateRequest();
	}

	HttpRequest->SetURL(TrimmedUrl);

	ProcessRequest();
}

UWorld *UVaRestRequestJSON::GetWorld() const
{
	UObject *Outer = GetOuter();
	return Outer ? Outer->GetWorld() : nullptr;
}

void UVaRestRequestJSON::ProcessRequest()
{
	// Set verb
	switch (RequestVerb)
	{
	case ERequestVerb::GET:
		HttpRequest->SetVerb(TEXT("GET"));
		break;
	case ERequestVerb::POST:
		HttpRequest->SetVerb(TEXT("POST"));
		break;
	case ERequestVerb::PUT:
		HttpRequest->SetVerb(TEXT("PUT"));
		break;
	case ERequestVerb::DEL:
		HttpRequest->SetVerb(TEXT("DELETE"));
		break;
	case ERequestVerb::CUSTOM:
		HttpRequest->SetVerb(CustomVerb);
		break;
	default: break;
	}

	// Set content-type
	switch (RequestContentType)
	{
	case ERequestContentType::x_www_form_urlencoded_url:
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

		FString UrlParams = "";
		uint16 ParamIdx = 0;

		// Loop through all the values and prepare additional url part
		for (auto RequestIt = RequestJsonObj->GetRootObject()->Values.CreateIterator(); RequestIt; ++RequestIt)
		{
			FString Key = RequestIt.Key();
			FString Value = RequestIt.Value().Get()->AsString();

			if (!Key.IsEmpty() && !Value.IsEmpty())
			{
				UrlParams += ParamIdx == 0 ? "?" : "&";
				UrlParams += UVaRestLibrary::PercentEncode(Key) + "=" + UVaRestLibrary::PercentEncode(Value);
			}

			ParamIdx++;
		}

		// Apply params
		HttpRequest->SetURL(HttpRequest->GetURL() + UrlParams);
		if (!StringRequestContent.IsEmpty())
		{
			HttpRequest->SetContentAsString(StringRequestContent);
		}

		UE_LOG(LogBusiness, Log, TEXT("Request (urlencoded): %s %s %s"), *HttpRequest->GetVerb(), *HttpRequest->GetURL(), *UrlParams, *StringRequestContent);
		break;
	}
	case ERequestContentType::x_www_form_urlencoded_body:
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

		FString UrlParams = "";
		uint16 ParamIdx = 0;

		// Loop through all the values and prepare additional url part
		for (auto RequestIt = RequestJsonObj->GetRootObject()->Values.CreateIterator(); RequestIt; ++RequestIt)
		{
			FString Key = RequestIt.Key();
			FString Value = RequestIt.Value().Get()->AsString();

			if (!Key.IsEmpty() && !Value.IsEmpty())
			{
				UrlParams += ParamIdx == 0 ? "" : "&";
				UrlParams += UVaRestLibrary::PercentEncode(Key) + "=" + UVaRestLibrary::PercentEncode(Value);
			}

			ParamIdx++;
		}

		// Apply params
		HttpRequest->SetContentAsString(UrlParams);

		UE_LOG(LogBusiness, Log, TEXT("Request (url body): %s %s %s"), *HttpRequest->GetVerb(), *HttpRequest->GetURL(), *UrlParams);

		break;
	}
	case ERequestContentType::binary:
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), BinaryContentType);
		HttpRequest->SetContent(RequestBytes);

		UE_LOG(LogBusiness, Log, TEXT("Request (binary): %s %s"), *HttpRequest->GetVerb(), *HttpRequest->GetURL());

		break;
	}
	case ERequestContentType::json:
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

		// Serialize data to json string
		FString OutputString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(RequestJsonObj->GetRootObject().ToSharedRef(), Writer);

		// Set Json content
		HttpRequest->SetContentAsString(OutputString);

		UE_LOG(LogBusiness, Log, TEXT("Request (json): %s %s %sJSON(%s%s%s)JSON"), *HttpRequest->GetVerb(), *HttpRequest->GetURL(), LINE_TERMINATOR, LINE_TERMINATOR, *OutputString, LINE_TERMINATOR);

		break;
	}
	default: break;
	}

	// Apply additional headers
	for (TMap<FString, FString>::TConstIterator It(RequestHeaders); It; ++It)
	{
		HttpRequest->SetHeader(It.Key(), It.Value());
	}
	
	// Bind event
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UVaRestRequestJSON::OnProcessRequestComplete);
	HttpRequest->ProcessRequest();

	++TryCount;
	GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &UVaRestRequestJSON::OnRequestTimeout, Timeout / 1000.0f, false);
}

void UVaRestRequestJSON::OnRequestTimeout()
{
	if (TryCount >= MaxTryCount)
	{
		OnStaticRequestFail.Broadcast(this);
		OnRequestFail.Broadcast(this);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle);
		TimeoutHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &UVaRestRequestJSON::OnRequestTimeout, Timeout / 1000.0f, false);
		
		++TryCount;
		HttpRequest->ProcessRequest();
	}
}

void UVaRestRequestJSON::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (TimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle);
		TimeoutHandle.Invalidate();
	}

	// Be sure that we have no data from previous response
	ResetResponseData();

	// Check we have a response and save response code as int32
	if(Response.IsValid())
	{
		ResponseCode = Response->GetResponseCode();
	}

	// Check we have result to process futher
	if (!bWasSuccessful || !Response.IsValid())
	{
		OnStaticRequestFail.Broadcast(this);
		OnRequestFail.Broadcast(this);
		UE_LOG(LogBusiness, Error, TEXT("Request failed (%d): %s"), ResponseCode, *Request->GetURL());
		return;
	}

	// Save response data as a string
	ResponseContent = Response->GetContentAsString();
	UE_LOG(LogBusiness, Log, TEXT("Response (%d): %sJSON(%s%s%s,url = %s%s)JSON"), ResponseCode, LINE_TERMINATOR, LINE_TERMINATOR, *ResponseContent, LINE_TERMINATOR, *Request->GetURL(), LINE_TERMINATOR);

	// Process response headers
	TArray<FString> Headers = Response->GetAllHeaders();
	for (FString Header : Headers)
	{
		FString Key;
		FString Value;
		if (Header.Split(TEXT(": "), &Key, &Value))
		{
			ResponseHeaders.Add(Key, Value);
		}
	}

	// Try to deserialize data to JSON
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseContent);
	FJsonSerializer::Deserialize(JsonReader, ResponseJsonObj->GetRootObject());

	// Decide whether the request was successful
	bIsValidJsonResponse = bWasSuccessful && ResponseJsonObj->GetRootObject().IsValid();
	if (!bIsValidJsonResponse)
	{
		if (!ResponseJsonObj->GetRootObject().IsValid())
		{
			UE_LOG(LogBusiness, Warning, TEXT("JSON could not be decoded!"));
		}
	}

	// Broadcast the result event
	OnStaticRequestComplete.Broadcast(this);
	OnRequestComplete.Broadcast(this);
}


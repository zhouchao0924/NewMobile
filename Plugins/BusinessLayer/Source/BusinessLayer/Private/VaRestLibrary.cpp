// Copyright 2016 Vladimir Alyamkin. All Rights Reserved.


#include "VaRestLibrary.h"
#include "VaRestRequestJSON.h"
#include "VaRestJsonObject.h"
#include "Base64.h"

//////////////////////////////////////////////////////////////////////////
// Helpers

FString UVaRestLibrary::PercentEncode(const FString& Source)
{
	FString OutText = Source;
	
	OutText = OutText.Replace(TEXT(" "), TEXT("%20"));
	OutText = OutText.Replace(TEXT("!"), TEXT("%21"));
	OutText = OutText.Replace(TEXT("\""), TEXT("%22"));
	OutText = OutText.Replace(TEXT("#"), TEXT("%23"));
	OutText = OutText.Replace(TEXT("$"), TEXT("%24"));
	OutText = OutText.Replace(TEXT("&"), TEXT("%26"));
	OutText = OutText.Replace(TEXT("'"), TEXT("%27"));
	OutText = OutText.Replace(TEXT("("), TEXT("%28"));
	OutText = OutText.Replace(TEXT(")"), TEXT("%29"));
	OutText = OutText.Replace(TEXT("*"), TEXT("%2A"));
	OutText = OutText.Replace(TEXT("+"), TEXT("%2B"));
	OutText = OutText.Replace(TEXT(","), TEXT("%2C"));
	OutText = OutText.Replace(TEXT("/"), TEXT("%2F"));
	OutText = OutText.Replace(TEXT(":"), TEXT("%3A"));
	OutText = OutText.Replace(TEXT(";"), TEXT("%3B"));
	OutText = OutText.Replace(TEXT("="), TEXT("%3D"));
	OutText = OutText.Replace(TEXT("?"), TEXT("%3F"));
	OutText = OutText.Replace(TEXT("@"), TEXT("%40"));
	OutText = OutText.Replace(TEXT("["), TEXT("%5B"));
	OutText = OutText.Replace(TEXT("]"), TEXT("%5D"));
	OutText = OutText.Replace(TEXT("{"), TEXT("%7B"));
	OutText = OutText.Replace(TEXT("}"), TEXT("%7D"));
	
	return OutText;
}

FString UVaRestLibrary::Base64Encode(const FString& Source)
{
	return FBase64::Encode(Source);
}

bool UVaRestLibrary::Base64Decode(const FString& Source, FString& Dest)
{
	return FBase64::Decode(Source, Dest);
}

bool UVaRestLibrary::Base64EncodeData(const TArray<uint8>& Data, FString& Dest)
{
	if (Data.Num() > 0)
	{
		Dest = FBase64::Encode(Data);
		return true;
	}
	
	return false;
}

bool UVaRestLibrary::Base64DecodeData(const FString& Source, TArray<uint8>& Dest)
{
	return FBase64::Decode(Source, Dest);
}

//////////////////////////////////////////////////////////////////////////
// Easy URL processing

TMap<UVaRestRequestJSON*, FVaRestCallResponse> UVaRestLibrary::RequestMap;

UVaRestRequestJSON* UVaRestLibrary::CallURL(UObject* WorldContextObject, const FString& URL, ERequestVerb Verb, ERequestContentType ContentType, UVaRestJsonObject* VaRestJson, const FVaRestCallDelegate& Callback)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		UE_LOG(LogBusiness, Error, TEXT("UVaRestLibrary: Wrong world context"))
		return nullptr;
	}

// 	UCEditorGameInstance* EditorGameInstance = Cast<UCEditorGameInstance>(World->GetGameInstance());
// 
// 	if (EditorGameInstance == nullptr)
// 	{
// 		UE_LOG(LogVaRest, Error, TEXT("UVaRestLibrary: Get editor game instance failure, can not get token."))
// 		return nullptr;
// 	}

	// Check we have valid data json
	if (VaRestJson == nullptr)
	{
		VaRestJson = UVaRestJsonObject::ConstructJsonObject(WorldContextObject);
	}
	
	UVaRestRequestJSON* Request = NewObject<UVaRestRequestJSON>();
	
	Request->SetVerb(Verb);
	Request->SetContentType(ContentType);
	Request->SetRequestObject(VaRestJson);
// 	Request->SetHeader(TEXT("Authorization"), EditorGameInstance->Token);
	
	FVaRestCallResponse Response;
	Response.Request = Request;
	Response.WorldContextObject = WorldContextObject;
	Response.Callback = Callback;
	
	Response.CompleteDelegateHandle = Request->OnStaticRequestComplete.AddStatic(&UVaRestLibrary::OnCallComplete);
	Response.FailDelegateHandle = Request->OnStaticRequestFail.AddStatic(&UVaRestLibrary::OnCallComplete);
	
	RequestMap.Add(Request, Response);
	
	Request->ResetResponseData();
	Request->ProcessURL(URL);

	return Request;
}

void UVaRestLibrary::CancelRequest(UVaRestRequestJSON *Request)
{
	if (Request && !Request->IsPendingKill())
	{
		FVaRestCallResponse* Response = RequestMap.Find(Request);
		if (Response)
		{
			Request->Cancel();
			RequestMap.Remove(Request);
			Request->ConditionalBeginDestroy();
		}
	}
}

void UVaRestLibrary::OnCallComplete(UVaRestRequestJSON* Request)
{
// 	if (!RequestMap.Contains(Request))
// 	{
// 		return;
// 	}
// 	
// 	FVaRestCallResponse* Response = RequestMap.Find(Request);
// 
// 	Request->OnStaticRequestComplete.Remove(Response->CompleteDelegateHandle);
// 	Request->OnStaticRequestFail.Remove(Response->FailDelegateHandle);
// 
// 	// Authority processing
// 	UVaRestJsonObject* ResponseObject = Request->GetResponseObject();
// 
// 	UWorld* World = GEngine->GetWorldFromContextObject(Response->WorldContextObject);
// 	UCEditorGameInstance* EditorGameInstance = Cast<UCEditorGameInstance>(World->GetGameInstance());
// 	if (World == nullptr)
// 	{
// 		UE_LOG(LogVaRest, Error, TEXT("UVaRestLibrary: Wrong world context"))
// 		Response->WorldContextObject = nullptr;
// 		Response->Request = nullptr;
// 		RequestMap.Remove(Request);
// 		return;
// 	}
// 
// 	if (EditorGameInstance == nullptr)
// 	{
// 		UE_LOG(LogBusiness, Error, TEXT("UVaRestLibrary: Get editor game instance failure, can not get token."))
// 		Response->WorldContextObject = nullptr;
// 		Response->Request = nullptr;
// 		RequestMap.Remove(Request);
// 		return;
// 	}
// 
// 	int32 Code = ResponseObject->GetNumberField(TEXT("code"));
// 
// 
// 
// 	if (Code == 111 || Code == 112 || Code == 113)
// 	{
// 		EditorGameInstance->HandleTokenError(Code, ResponseObject->GetStringField(TEXT("msg")));
// 	}
// 	else
// 	{
// 
// 		Response->Callback.ExecuteIfBound(Request);
// 	}
// 	
// 	Response->WorldContextObject = nullptr;
// 	Response->Request = nullptr;
// 	RequestMap.Remove(Request);
}

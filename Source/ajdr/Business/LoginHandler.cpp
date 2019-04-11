// Fill out your copyright notice in the Description page of Project Settings.

#include "ajdr.h"
#include "DRGameMode.h"
#include "VaRestJsonObject.h"

void ADRGameMode::LoginHandler(UVaRestJsonObject *ResponseObj)
{
	FString JsonStr = ResponseObj->EncodeJson();

	if (ResponseObj->GetBoolField(TEXT("success")))
	{
		FString Token = ResponseObj->GetStringField(TEXT("token"));
		Business->SetToken(Token);
		SetState(EState::ELogin);
	}
	else
	{
		SetState(EState::EOffline);
	}
}



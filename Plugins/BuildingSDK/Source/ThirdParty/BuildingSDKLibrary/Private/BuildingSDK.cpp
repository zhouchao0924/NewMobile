
#include "BuildingSDK.h"
#include "ISuite.h"
#include "MXFile/MXFile.h"
#include "Stream/FileStream.h"
#include "TargetPlatform/PCPlatform.h"
#include "TargetPlatform/AndroidPlatform.h"

IValueFactory *GValueFactory = NULL;

BuildingSDKImpl::BuildingSDKImpl()
	:_bInitialized(false)
	,_Transaction(nullptr)
{
}

bool BuildingSDKImpl::Initialize()
{
	_bInitialized = true;
	
	_Transaction = new FTransaction();
	GValueFactory = &_ValueFactoy;

	_Platforms.push_back(new PCPlatform());
	_Platforms.push_back(new AndroidPlatform());

	return true;
}

void BuildingSDKImpl::UnInitialize()
{
	for (size_t i = 0; i < _Suites.size(); ++i)
	{
		SuiteImpl *Suite = _Suites[i];
		delete Suite;
	}
	
	_bInitialized = false; 
	GValueFactory = nullptr;

	if (_Transaction)
	{
		delete _Transaction;
		_Transaction = nullptr;
	}
}

ITransact *BuildingSDKImpl::GetTransaction()
{
	return _Transaction;
}

void BuildingSDKImpl::SetTransaction(ITransact *InTransaction)
{
	_Transaction = InTransaction;
}

ISuite *BuildingSDKImpl::CreateSuite()
{
#if	!USE_MX_ONLY
	if (_bInitialized)
	{
		SuiteImpl *Suite = new SuiteImpl();
		_Suites.push_back(Suite);
		return Suite;
	}
#endif
	return nullptr;
}

void BuildingSDKImpl::DestroySuite(ISuite *Suite)
{
#if !USE_MX_ONLY
	SuiteImpl *SuiteImp = (SuiteImpl*)Suite;
	if (SuiteImp)
	{
		for (size_t i = 0; i < _Suites.size(); ++i)
		{
			if (_Suites[i] == SuiteImp)
			{
				_Suites.erase(_Suites.begin() + i);
				break;
			}
		}
		delete SuiteImp;
	}
#endif
}

ISuite *BuildingSDKImpl::LoadSuite(const char *Filename)
{
#if !USE_MX_ONLY
	if (_bInitialized)
	{
		SuiteImpl *Suite = (SuiteImpl *)CreateSuite();

		if (!Suite->Load(Filename))
		{
			DestroySuite(Suite);
			Suite = nullptr;
		}
		
		return Suite;
	}
#endif
	return nullptr;
}

ISerialize *BuildingSDKImpl::CreateFileWriter(const char *Filename)
{
	return new FileWriter(Filename);
}

ISerialize *BuildingSDKImpl::CreateFileReader(const char *Filename)
{
	return new FileReader(Filename);
}

IObject *BuildingSDKImpl::LoadFile(const char *Filename)
{
	return MXFile::LoadFromFile(Filename);
}

IValueFactory *BuildingSDKImpl::GetValueFactory()
{
	return &_ValueFactoy;
}

ITargetPlatform *BuildingSDKImpl::GetPlatform(ETargetPlatform PlatformType)
{
	for (size_t i = 0; i < _Platforms.size(); ++i)
	{
		if (_Platforms[i]->GetPlatformType() == PlatformType)
		{
			return _Platforms[i];
		}
	}
	return nullptr;
}

IBuildingSDK *GetBuildingSDK()
{
	static BuildingSDKImpl sdk;
	return  &sdk;
}

extern "C" _declspec(dllexport) void * LoadSDKFunction()
{
	return GetBuildingSDK();
}


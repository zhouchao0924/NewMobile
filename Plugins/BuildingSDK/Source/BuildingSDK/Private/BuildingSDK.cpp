// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BuildingSDK.h"
#include "Core.h"
#include "IBuildingSDK.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "BuildingSDKLibrary/BuildingSDKLibrary.h"

#define LOCTEXT_NAMESPACE "FBuildingSDKModule"

FBuildingSDKModule::FBuildingSDKModule()
	: BuildingSDKLibraryHandle(nullptr)
	, SDK(nullptr)
{
}

typedef void * (*LoadSDKFunctionType) ();

void FBuildingSDKModule::StartupModule()
{
}

void FBuildingSDKModule::ShutdownModule()
{
	if (SDK)
	{
		SDK->UnInitialize();
		SDK = nullptr;
	}
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	// Free the dll handle
	FPlatformProcess::FreeDllHandle(BuildingSDKLibraryHandle);
	BuildingSDKLibraryHandle = nullptr;
}

IBuildingSDK *FBuildingSDKModule::GetSDK()
{
	if (!SDK)
	{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// Get the base directory of this plugin
		FString BaseDir = IPluginManager::Get().FindPlugin("BuildingSDK")->GetBaseDir();

		// Add on the relative location of the third party dll and load it
		FString LibraryPath;
#if PLATFORM_WINDOWS
		LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/BuildingSDKLibrary/Win64/BuildingSDKLibrary.dll"));
#elif PLATFORM_MAC
		LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/BuildingSDKLibrary/Mac/Release/BuildingSDKLibrary.dylib"));
#endif // PLATFORM_WINDOWS
		BuildingSDKLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
		if (BuildingSDKLibraryHandle)
		{
			LoadSDKFunctionType Func = (LoadSDKFunctionType)FPlatformProcess::GetDllExport(BuildingSDKLibraryHandle, TEXT("LoadSDKFunction"));
			// Call the test function in the third party library that opens a message box
			SDK = (IBuildingSDK *)GetBuildingSDKFunction();
			if (SDK)
			{
				SDK->Initialize();
			}
		}
	}

	return SDK;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBuildingSDKModule, BuildingSDK)


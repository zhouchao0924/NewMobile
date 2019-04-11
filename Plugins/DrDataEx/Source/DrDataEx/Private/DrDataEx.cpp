// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DrDataEx.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "DrDataExLibrary.h"

#define LOCTEXT_NAMESPACE "FDrDataExModule"

void FDrDataExModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("DrDataEx")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/DrDataExLibrary/Win64/DrDataEx.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/DrDataExLibrary/Mac/Release/libDrDataEx.dylib"));
#endif // PLATFORM_WINDOWS

	DrDataExLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if(DrDataExLibraryHandle)
	{
		int c = 0;
		c++;
	} 
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FDrDataExModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	// Free the dll handle
	FPlatformProcess::FreeDllHandle(DrDataExLibraryHandle);
	DrDataExLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDrDataExModule, DrDataEx)



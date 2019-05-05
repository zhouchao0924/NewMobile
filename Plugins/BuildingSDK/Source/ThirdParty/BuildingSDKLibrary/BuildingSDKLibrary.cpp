
#include "Private/BuildingSDK.h"

#if defined _WIN32 || defined _WIN64
    #include <Windows.h>

    #define DLLEXPORT __declspec(dllexport)
#else
    #include <stdio.h>
#endif

#ifndef DLLEXPORT
    #define DLLEXPORT
#endif


DLLEXPORT void* GetBuildingSDKFunction()
{
	static BuildingSDKImpl sdk;
	return &sdk;
}


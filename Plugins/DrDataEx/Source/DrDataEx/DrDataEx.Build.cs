// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class DrDataEx : ModuleRules
{
	public DrDataEx(ReadOnlyTargetRules Target): base(Target)
	{
        //      PublicIncludePaths.AddRange(
        //	new string[] {
        //		"DrDataEx/Public"
        //		// ... add public include paths required here ...
        //	}
        //	);


        //PrivateIncludePaths.AddRange(
        //	new string[] {
        //		"DrDataEx/Private",
        //		// ... add other private include paths required here ...
        //	}
        //	);
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DrDataExLibrary",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
    }
}

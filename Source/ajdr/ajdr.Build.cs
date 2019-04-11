
using System.IO;
using UnrealBuildTool;

public class ajdr : ModuleRules
{
    public ajdr(ReadOnlyTargetRules Target) : base(Target)
    {

        bEnableUndefinedIdentifierWarnings = false;
        PublicDependencyModuleNames.AddRange(new string[]
        {
          "Core",
          "CoreUObject",
          "Engine",
          "InputCore",
          "ProceduralMeshComponent",
          "APPFRAMEWORK" ,
          "RHI",
          "RenderCore",
          "UMG",
          "ImageWrapper",
          "NetworkFile",
          "HTTP",
          "TextureCompressor",
          "ImageCore",
          "FBX",
          "Json",
          "JsonUtilities",
          "TargetPlatform",
          "Sockets",
          "Networking",
          "BuildingSDK",
          "BusinessLayer"              
        });

        PublicIncludePaths.AddRange(
           new string[] {
               Path.Combine(ModuleDirectory, "Viewport"),
               Path.Combine(ModuleDirectory, "EditMode"),
               Path.Combine(ModuleDirectory, "UMG"),
               Path.Combine(ModuleDirectory, "Actor"),
               Path.Combine(ModuleDirectory, "Resource"),
               Path.Combine(ModuleDirectory, "Common"),
               Path.Combine(ModuleDirectory, "Common/Network"),
               Path.Combine(ModuleDirectory, "Common/AsyncTask"),
           }
           );

        if (Target.Type == TargetRules.TargetType.Server || Target.Type == TargetRules.TargetType.Editor)
        {
//          PublicDependencyModuleNames.Add("WebServer");
//          PublicDependencyModuleNames.Add("UGW_WebPanoramaPlugin");
//          AddEngineThirdPartyPrivateStaticDependencies(Target, "libcurl");
        }

//      AddEngineThirdPartyPrivateStaticDependencies(Target, "libcurl");
//      DynamicallyLoadedModuleNames.AddRange(new string[] { "eXiSoundVis" });
//      DynamicallyLoadedModuleNames.AddRange(new string[] { "PeExtendedToolKit" });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "TextureFormatDXT",
                "nvTextureTools",
                "VHACD",
            }
        );

        if (Target.bBuildEditor == true)//如果是编辑器
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                }
            );
        }


        //PrivatePCHHeaderFile = "ajdr.h";

        //"UGW_WebPanoramaPlugin",

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        PublicDefinitions.Add("BOOST_SYSTEM_NOEXCEPT");
        bEnableExceptions = true;
        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}

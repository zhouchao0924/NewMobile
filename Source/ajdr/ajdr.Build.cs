
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
          //"TextureCompressor",
          "ImageCore",
          "FBX",
          "Json",
          "JsonUtilities",
          //"TargetPlatform",
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
        }

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
        PublicDefinitions.Add("BOOST_SYSTEM_NOEXCEPT");
        bEnableExceptions = true;
    }
}

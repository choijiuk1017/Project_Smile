// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class Project_Smile : ModuleRules
{
	public Project_Smile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ "Core",
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput" ,
			"RenderCore",
			"RHI",
			"ImageWrapper",
			"ImageWriteQueue",
            "HTTP",
			"Json",
			"JsonUtilities",
			"UMG",
			"Slate",
			"SlateCore",
			"AIModule",
            "GameplayTasks",
            "NavigationSystem"
        });

        PublicIncludePaths.Add("Project_Smile");
    }
}

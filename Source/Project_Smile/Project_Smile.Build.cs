// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_Smile : ModuleRules
{
	public Project_Smile(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}

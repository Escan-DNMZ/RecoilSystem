// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RecoilSystem : ModuleRules
{
	public RecoilSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AutobattlerPlugin : ModuleRules
{
	public AutobattlerPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"OnlineSubsystem",
				"Slate",
				"SlateCore",
				"UMG",
				"ProceduralMeshComponent",
				"DeveloperSettings",
				"AIModule",
				"GameplayTasks",
				"OnlineSubsystem",
				"NavigationSystem"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
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

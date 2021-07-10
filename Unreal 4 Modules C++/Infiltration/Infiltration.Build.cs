using UnrealBuildTool;
 
public class Infiltration : ModuleRules
{
	public Infiltration(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "GameplayTasks" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"Infiltration/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"Infiltration/Private"});
	}
}

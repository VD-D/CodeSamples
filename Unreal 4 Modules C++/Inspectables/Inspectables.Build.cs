using UnrealBuildTool;
 
public class Inspectables : ModuleRules
{
	public Inspectables(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"Inspectables/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"Inspectables/Private"});
	}
}

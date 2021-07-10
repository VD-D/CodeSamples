using UnrealBuildTool;
 
public class Locomotion : ModuleRules
{
	public Locomotion(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"Locomotion/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"Locomotion/Private"});
	}
}

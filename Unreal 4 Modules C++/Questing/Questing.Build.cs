using UnrealBuildTool;
 
public class Questing : ModuleRules
{
	public Questing(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
 
		PublicIncludePaths.AddRange(new string[] {"Questing/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"Questing/Private"});
	}
}

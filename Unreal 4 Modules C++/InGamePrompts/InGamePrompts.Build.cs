using UnrealBuildTool;
 
public class InGamePrompts : ModuleRules
{
	public InGamePrompts(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
 
		PublicIncludePaths.AddRange(new string[] {"InGamePrompts/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"InGamePrompts/Private"});
	}
}

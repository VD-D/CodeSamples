using UnrealBuildTool;
 
public class TopDownDialogues : ModuleRules
{
	public TopDownDialogues(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"TopDownDialogues/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"TopDownDialogues/Private"});
	}
}

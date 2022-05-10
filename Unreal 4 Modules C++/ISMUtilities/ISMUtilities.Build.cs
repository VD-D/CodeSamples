using UnrealBuildTool;
 
public class ISMUtilities : ModuleRules
{
	public ISMUtilities(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"ISMUtilities/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"ISMUtilities/Private"});
	}
}

using UnrealBuildTool;
 
public class LineOfSightVisualisation : ModuleRules
{
	public LineOfSightVisualisation(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"LineOfSightVisualisation/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"LineOfSightVisualisation/Private"});
	}
}

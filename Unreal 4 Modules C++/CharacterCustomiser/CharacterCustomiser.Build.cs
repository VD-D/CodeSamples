using UnrealBuildTool;
 
public class CharacterCustomiser : ModuleRules
{
	public CharacterCustomiser(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
 
		PublicIncludePaths.AddRange(new string[] {"CharacterCustomiser/Public"});
 		PrivateIncludePaths.AddRange(new string[] {"CharacterCustomiser/Private"});

        // bLegacyPublicIncludePaths = false;
    }
}

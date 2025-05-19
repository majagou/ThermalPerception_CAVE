using UnrealBuildTool;
using System.IO;

public class V2C_Utilities : ModuleRules
{
	public V2C_Utilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange
		(
			new string[] {Path.Combine(ModuleDirectory, "Public"),}
		);
				
		
		PrivateIncludePaths.AddRange
		(
			new string[] {Path.Combine(ModuleDirectory, "Private"),}
		);
			
		
		PublicDependencyModuleNames.AddRange
		(
			new string[] {"Core", "Sockets", "Networking"}
		);
			
		
		PrivateDependencyModuleNames.AddRange
		(
			new string[] {"CoreUObject", "Engine", "Slate",	"SlateCore"}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange
		(
			new string[] {	}
		);
	}
}
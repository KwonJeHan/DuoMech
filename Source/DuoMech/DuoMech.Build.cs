// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DuoMech : ModuleRules
{
	public DuoMech(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "DuoMech" });
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject", 
			"Engine",
			"InputCore", 
			"EnhancedInput",
			"AIModule",
			"Niagara",
			"UMG"
		});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { 
			"Slate",
			"SlateCore"
		});

        // 네트워크 멀티플레이어 지원을 위한 모듈 추가
        PublicDependencyModuleNames.AddRange(new string[] {
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
        });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}

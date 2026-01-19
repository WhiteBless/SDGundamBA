// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SDGundamBA : ModuleRules
{
	public SDGundamBA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"MotionWarping"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SDGundamBA",
			"SDGundamBA/Variant_Platforming",
			"SDGundamBA/Variant_Platforming/Animation",
			"SDGundamBA/Variant_Combat",
			"SDGundamBA/Variant_Combat/AI",
			"SDGundamBA/Variant_Combat/Animation",
			"SDGundamBA/Variant_Combat/Gameplay",
			"SDGundamBA/Variant_Combat/Interfaces",
			"SDGundamBA/Variant_Combat/UI",
			"SDGundamBA/Variant_SideScrolling",
			"SDGundamBA/Variant_SideScrolling/AI",
			"SDGundamBA/Variant_SideScrolling/Gameplay",
			"SDGundamBA/Variant_SideScrolling/Interfaces",
			"SDGundamBA/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

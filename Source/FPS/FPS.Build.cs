// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPS : ModuleRules
{
	public FPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"EngineSettings",
				"EnhancedInput",
				"UMG",
				"PhysicsCore",
				"Chaos",
				"Networking",
				"ProceduralMeshComponent",
				"RenderCore",
				"RHI",
				"MediaAssets",
				"HTTP",
				"Json",
				"JsonUtilities",
				"UMG",
				// Drone combat prototype: AAIController / bot logic lives in Drone/.
				"AIModule",

				// Human combat: RealisticBlood is pure Niagara; bots path on navmesh.
				"Niagara",
				"NavigationSystem",
				"ALS",
				"GameplayTags"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"EngineCameras",
				"Slate",
				"SlateCore",
				"Sockets",
				"OnlineSubsystem",
				"OnlineSubsystemUtils"
			}
		);
		PublicIncludePaths.AddRange(new string[] {
			"FPS"
		});
	}
}

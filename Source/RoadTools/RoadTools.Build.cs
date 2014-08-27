// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class RoadTools : ModuleRules
	{
		public RoadTools(TargetInfo Target)
		{
			/*
			PublicIncludePaths.AddRange(
				new string[] {
                    "RoadTools/Public"
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"RoadTools/Private",
					// ... add other private include paths required here ...
				}
				);
			*/
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "CoreUObject",
                    "Engine",
                    //"UnrealEd",
					// ... add other public dependencies that you statically link with here ...
				}
				);

			/*
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
			*/
		}
	}
}
/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
using System.IO;
using UnrealBuildTool;

public class CloudConnector : ModuleRules {

	public CloudConnector(ReadOnlyTargetRules Target) : base(Target) {

		PrivatePCHHeaderFile = "Private/CloudConnectorPrivatePCH.h";

		bEnableExceptions = true;  // Google Cloud SDK only has exceptions, so we enable this
		CppStandard = CppStandardVersion.Cpp17;
		PrivateDefinitions.Add("_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING");
		PrivateDefinitions.Add("_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING");

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"AWS_SDK",
			"GoogleCloud_SDK",
			"HTTP",
			"Json",
			"JsonUtilities",
			"Http"
		});
	}
}

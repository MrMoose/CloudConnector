/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
using System.IO;
using UnrealBuildTool;

public class CloudConnector : ModuleRules {

	/* The Google Cloud SDK links statically against libprotobuf (via grpc)
	 * So does webrtc (via PixelStreaming plugin) but against a much older
	 * version. Those two cannot coexist in one process. See here:
	 * https://github.com/MrMoose/CloudConnector/issues/2
	 * 
	 * Using this value you can inhibit usage of the Google Cloud SDK.
	 * Of course in this case you are restricted to AWS only. 
	 * Nothing else will work.
	 * 
	 * If you have further information about this or know how to safely link
	 * both, please contact MrMoose
	 */
	bool SupportGoogleCloud = true;

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
			"Json",
			"JsonUtilities",
			"Http"
		});

		if (SupportGoogleCloud) {
			PrivateDependencyModuleNames.AddRange(new string[] {
				"GoogleCloud_SDK"
			});
		}

	}
}

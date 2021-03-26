using System;
using System.IO;
using UnrealBuildTool;

/**
 * This is a binary only third party build of the AWS C++ SDK.
 * https://docs.unrealengine.com/en-US/Programming/BuildTools/UnrealBuildTool/ThirdPartyLibraries/index.html
 */
public class AWS_SDK : ModuleRules {

    public AWS_SDK(ReadOnlyTargetRules Target) : base(Target) {

		Type = ModuleType.External;

		// During Link, some static symbols in 
		// the SDK weren't found. This fixes it but it should always be the case for DLL linkage
		PublicDefinitions.Add("USE_IMPORT_EXPORT");

		// add any macros that need to be set. This is just an example define.
		PublicDefinitions.Add("WITH_AWS_SDK=1");

		// include path is public for the module. Can it be private and 
		// still be used by other modules of the same plugin?
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

		// configure link libraries. This basically makes it windows x64 only
		string LibrariesPath = Path.Combine(ModuleDirectory, "lib", "Win64");

		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-c-common.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-c-event-stream.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-checksums.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-access-management.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-cognito-identity.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-core.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-iam.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-logs.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-monitoring.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-s3.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-sqs.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "aws-cpp-sdk-xray.lib"));

		// add the appropriate DLLs to the package by 
		// copying the DDLs to the appropriate output dir.
		// Note that during packing, they end up in the project's binary dir as well
		// and won't be replaced upon update
		string DLLPath = Path.Combine(ModuleDirectory, "bin", "Win64");

		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-c-common.dll", Path.Combine(DLLPath, "aws-c-common.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-c-event-stream.dll", Path.Combine(DLLPath, "aws-c-event-stream.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-checksums.dll", Path.Combine(DLLPath, "aws-checksums.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-access-management.dll", Path.Combine(DLLPath, "aws-cpp-sdk-access-management.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-cognito-identity.dll", Path.Combine(DLLPath, "aws-cpp-sdk-cognito-identity.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-core.dll", Path.Combine(DLLPath, "aws-cpp-sdk-core.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-iam.dll", Path.Combine(DLLPath, "aws-cpp-sdk-iam.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-logs.dll", Path.Combine(DLLPath, "aws-cpp-sdk-logs.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-monitoring.dll", Path.Combine(DLLPath, "aws-cpp-sdk-monitoring.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-s3.dll", Path.Combine(DLLPath, "aws-cpp-sdk-s3.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-sqs.dll", Path.Combine(DLLPath, "aws-cpp-sdk-sqs.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/aws-cpp-sdk-xray.dll", Path.Combine(DLLPath, "aws-cpp-sdk-xray.dll"));
	}
}

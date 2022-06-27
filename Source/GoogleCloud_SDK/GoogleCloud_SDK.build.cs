using System;
using System.IO;
using UnrealBuildTool;

/**
 * This is a binary only third party build of the Google Cloud C++ SDK.
 * https://docs.unrealengine.com/en-US/Programming/BuildTools/UnrealBuildTool/ThirdPartyLibraries/index.html
 */
public class GoogleCloud_SDK : ModuleRules {

    public GoogleCloud_SDK(ReadOnlyTargetRules Target) : base(Target) {

		Type = ModuleType.External;

		CppStandard = CppStandardVersion.Cpp17;

		// add any macros that need to be set. This is just an example define.
		PublicDefinitions.Add("WITH_GOOGLECLOUD_SDK=1");

		// include path is public for the module. Can it be private and 
		// still be used by other modules of the same plugin?
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

		// configure link libraries. This basically makes it windows x64 only
		string LibrariesPath = Path.Combine(ModuleDirectory, "lib", "Win64");

		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_annotations_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_auth_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_client_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_distribution_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_field_behavior_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_http_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_label_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_launch_stage_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_metric_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_monitored_resource_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_api_resource_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_common.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_devtools_cloudtrace_v2_trace_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_devtools_cloudtrace_v2_tracing_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_grpc_utils.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_iam.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_iam_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_iam_v1_iam_policy_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_iam_v1_options_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_iam_v1_policy_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_logging.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_logging_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_logging_type_type_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_longrunning_operations_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_monitoring_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_pubsub.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_pubsub_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_rest_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_rpc_error_details_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_rpc_status_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_storage.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_storage_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_calendar_period_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_color_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_datetime_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_date_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_dayofweek_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_expr_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_fraction_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_interval_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_latlng_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_localized_text_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_money_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_month_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_phone_number_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_postal_address_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_quaternion_protos.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "google_cloud_cpp_type_timeofday_protos.lib"));

		// Now all those are indirect dependencies. As all google stuff links statically,
		// I have to pull them in as well. Sadly not knowing which ones are actually needed.
		// They are all compiled manually to make sure they match with one another.
		// All of them should be /MD, linked statically. None should require any DLL.

		// Abseil.
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_bad_any_cast_impl.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_bad_optional_access.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_bad_variant_access.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_base.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_city.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_civil_time.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_cord.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_cord_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_cordz_functions.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_cordz_handle.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_cordz_info.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_cordz_sample_token.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_debugging_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_demangle_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_examine_stack.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_exponential_biased.lib"));
	
		/* Some are not needed as determined by trial and error
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_failure_signal_handler.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_commandlineflag.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_commandlineflag_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_config.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_marshalling.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_parse.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_private_handle_accessor.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_program_name.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_reflection.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_usage.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_flags_usage_internal.lib"));
		*/

		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_graphcycles_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_hash.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_low_level_hash.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_hashtablez_sampler.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_int128.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_malloc_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_raw_hash_set.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_raw_logging_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_distributions.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_platform.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_pool_urbg.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_randen.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_randen_hwaes.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_randen_hwaes_impl.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_randen_slow.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_internal_seed_material.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_random_seed_gen_exception.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_scoped_set_env.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_spinlock_wait.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_stacktrace.lib"));		
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_status.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_statusor.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_strerror.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_strings.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_strings_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_str_format_internal.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_symbolize.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_synchronization.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_throw_delegate.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_time.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "absl_time_zone.lib"));

		// OpenSSL
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libcrypto.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libssl.lib"));
		PublicSystemLibraries.Add("crypt32.lib");   // indirect dependencies coming in via OpenSSL. Windows only.
		PublicSystemLibraries.Add("Wldap32.lib");

		// GRPC
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "address_sorting.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "cares.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "gpr.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc++.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc++_alts.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc++_error_details.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc++_reflection.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc++_unsecure.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpcpp_channelz.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc_plugin_support.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "grpc_unsecure.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "re2.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "upb.lib"));

		// Protobuf
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libprotobuf.lib"));

		// CURL
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libcurl.lib"));

		// Crc32c
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "crc32c.lib"));

		// Ssh2
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libssh2.lib"));

		// ZLib
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "zlibstat.lib"));


	}
}

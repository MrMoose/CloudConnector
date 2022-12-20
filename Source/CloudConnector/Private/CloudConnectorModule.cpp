/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudConnectorModule.h"
#include "CloudConnector.h"
#include "BlindStorageImpl.h"
#include "BlindQueueImpl.h"
#include "BlindPubsubImpl.h"
#include "BlindTracingImpl.h"
#include "BlindMetricsImpl.h"
#include "AWSMemoryManager.h"
#include "AWSStorageImpl.h"
#include "AWSQueueImpl.h"
#include "AWSPubsubImpl.h"
#include "AWSTracingImpl.h"
#include "AWSMetricsImpl.h"
#include "GoogleCloudStorageImpl.h"
#include "GoogleQueueImpl.h"
#include "GooglePubsubImpl.h"
#include "OpenTelemetryTracingImpl.h"
#include "OpenTelemetryMetricsImpl.h"
#include "Utilities.h"
#include "CloudWatchLogOutputDevice.h"
#include "GoogleLoggingOutputDevice.h"

#include "Windows/PreWindowsApi.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include "Windows/PostWindowsApi.h"

#include "Engine/World.h"   // for optional suppression of CloudWatch when PIE
#include "Misc/OutputDeviceFile.h"

#include <atomic>

DEFINE_LOG_CATEGORY(LogCloudConnector)
#define LOCTEXT_NAMESPACE "FCloudConnectorModule"


std::atomic<bool>  s_aws_sdk_initialized = false;
Aws::SDKOptions    s_aws_sdk_options;


void FCloudConnectorModule::StartupModule() {

	UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector Plugin"));
}

void FCloudConnectorModule::ShutdownModule() {

	UE_LOG(LogCloudConnector, Display, TEXT("Shutting down Cloud Connector Plugin"));
}

bool FCloudConnectorModule::SupportsDynamicReloading() {

	return false;
}

void FCloudConnectorModule::init_actor_config(const ACloudConnector *n_config) {
	
	if (n_config) {
		m_provider = cloud_provider(n_config->CloudProvider);

		switch (m_provider) {
			default:
			case ECloudProvider::BLIND:
				UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector in Blind mode"));
				m_storage = MakeUnique<BlindStorageImpl>();
				m_queue = MakeUnique<BlindQueueImpl>();
				m_pubsub = MakeUnique<BlindPubsubImpl>();
				m_tracing = MakeUnique<BlindTracingImpl>();
				m_metrics = MakeUnique<BlindMetricsImpl>();
				break;

			case ECloudProvider::AWS:
				UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector in AWS mode"));

				// Yes, this will cause trouble if more than one module links against
				// the AWS SDK but at this point I'll have to deal with that later
				if (!s_aws_sdk_initialized) {
					s_aws_sdk_options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
					// Yes, a dangling memory manager. Let's see who catches this ;-)
					s_aws_sdk_options.memoryManagementOptions.memoryManager = new AWSMemoryManager();
					Aws::InitAPI(s_aws_sdk_options);
					s_aws_sdk_initialized.store(true);
				}

				if (n_config->AWSLogs) {
					// You won't need logging in live system. This is file IO after all.
					// Disable this for production unless you need it
					Aws::Utils::Logging::InitializeAWSLogging(
						Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(
							"CCAllocationTag", Aws::Utils::Logging::LogLevel::Info, "aws_sdk_"));
				}

				// If the user wants logs, an outputdevice is injected into the engine logs
				if (logs_enabled(n_config->CloudLogs)) {

					// ... but not when PIE if so requested. This is a feature only requested by one 
					// so I put it in with as little complexity as possible. Everything else shall work
					// as intended, except CloudWatch.
					// #moep remove this when no longer required
					if (!(n_config->AWSSuppressCloudWatchLogsInPIE && n_config->GetWorld()->IsPlayInEditor())) {
						m_log_device = MakeUnique<FCloudWatchLogOutputDevice>(n_config->LogGroupPrefix, 
								static_cast<ELogVerbosity::Type>(n_config->AWSCloudWatchVerbosityLimit));
						GLog->AddOutputDevice(m_log_device.Get());
					}
				}

				// If tracing is not enabled, we use the blind tracing impl to be able to do all that 
				// without code changes
				if (tracing_enabled(n_config->Tracing)) {
					m_tracing = MakeUnique<AWSTracingImpl>();
				} else {
					m_tracing = MakeUnique<BlindTracingImpl>();
				}

				m_storage = MakeUnique<AWSStorageImpl>();
				m_queue = MakeUnique<AWSQueueImpl>(n_config);
				m_pubsub  = MakeUnique<AWSPubsubImpl>(n_config);
				m_metrics = MakeUnique<AWSMetricsImpl>(n_config);
				break;

			case ECloudProvider::GOOGLE:
				UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector in Google mode"));
				m_storage = MakeUnique<GoogleCloudStorageImpl>();
#ifdef WITH_GOOGLECLOUD_SDK
				m_queue = MakeUnique<GoogleQueueImpl>(n_config);
				m_pubsub  = MakeUnique<GooglePubsubImpl>(n_config);

				if (logs_enabled(n_config->CloudLogs)) {
					m_log_device = MakeUnique<FGoogleLoggingOutputDevice>(n_config->GoogleProjectId);
					GLog->AddOutputDevice(m_log_device.Get());
				}
#else
				m_queue = MakeUnique<BlindQueueImpl>();
				m_pubsub = MakeUnique<BlindPubsubImpl>();
#endif

				if (tracing_enabled(n_config->Tracing)) {
					m_tracing = MakeUnique<OpenTelemetryTracingImpl>();
				} else {
					m_tracing = MakeUnique<BlindTracingImpl>();
				}

				break;
		}
	} else {
		switch (m_provider) {
			default:
			case ECloudProvider::BLIND:
				UE_LOG(LogCloudConnector, Display, TEXT("Stopping blind Cloud Connector"));
				break;

			case ECloudProvider::AWS:
				UE_LOG(LogCloudConnector, Display, TEXT("Stopping AWS Cloud Connector"));
				if (m_log_device) {
					GLog->RemoveOutputDevice(m_log_device.Get());
					m_log_device.Reset();  // Calling d'tor and joining thread.
				}

				m_queue->shutdown();
				m_pubsub->shutdown();

				// I am not deleting this object as the engine should call TearDown()
				// upon it to release resources
				if (s_aws_sdk_initialized) {
					Aws::Utils::Logging::ShutdownAWSLogging();

					// Again, this will cause trouble if more than one module links against the AWS SDK.
					// I am well aware of this.
					// Also, I have to disable shutting down the API to mitigate the effects of 
					// https://github.com/MrMoose/CloudConnector/issues/3
					// It will crash when using SQS because there's no way of shutting  down outstanding 
					// requests gracefully
					Aws::ShutdownAPI(s_aws_sdk_options);
					s_aws_sdk_initialized.store(false);
				}

				break;

			case ECloudProvider::GOOGLE:
				UE_LOG(LogCloudConnector, Display, TEXT("Stopping Google Cloud Connector"));
				
				m_queue->shutdown();
				m_pubsub->shutdown();

				if (m_log_device) {
					GLog->RemoveOutputDevice(m_log_device.Get());
					m_log_device.Reset();  // Calling d'tor and joining thread.
				}

				break;
		}

		m_provider = ECloudProvider::BLIND;
		//m_storage.Reset();
		//m_pubsub.Reset();
		//m_tracing.Reset();
	}
}

ICloudStorage &FCloudConnectorModule::storage() const {

	checkf(m_storage, TEXT("You are calling this too early, please wait for the game to start"))
	return *m_storage;
}

ICloudQueue &FCloudConnectorModule::queue() const {

	checkf(m_queue, TEXT("You are calling this too early or too late, please wait for the game to start"))
	return *m_queue;
}

ICloudPubsub &FCloudConnectorModule::pubsub() const {

	checkf(m_pubsub, TEXT("You are calling this too early or too late, please wait for the game to start"))
	return *m_pubsub;
}

ICloudTracing &FCloudConnectorModule::tracing() const {

	checkf(m_tracing, TEXT("You are calling this too early or too late, please wait for the game to start"))
	return *m_tracing;
}

ICloudMetrics &FCloudConnectorModule::metrics() const {

	checkf(m_metrics, TEXT("You are calling this too early or too late, please wait for the game to start"))
	return *m_metrics;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FCloudConnectorModule, CloudConnector)

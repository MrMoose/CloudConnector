/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudConnectorModule.h"
#include "CloudConnector.h"
#include "BlindStorageImpl.h"
#include "BlindPubsubImpl.h"
#include "AWSStorageImpl.h"
#include "AWSPubsubImpl.h"
#include "GoogleCloudStorageImpl.h"
#include "GooglePubsubImpl.h"
#include "Utilities.h"
#include "CloudWatchLogOutputDevice.h"

#include "Windows/PreWindowsApi.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include "Windows/PostWindowsApi.h"


DEFINE_LOG_CATEGORY(LogCloudConnector)
#define LOCTEXT_NAMESPACE "FCloudConnectorModule"


TAtomic<bool>    s_aws_sdk_initialized = false;
Aws::SDKOptions  s_aws_sdk_options;


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
		m_provider = n_config->CloudProvider;

		switch (m_provider) {
			default:
			case ECloudProvider::BLIND:
				UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector in Blind mode"));
				m_storage = MakeUnique<BlindStorageImpl>();
				m_pubsub = MakeUnique<BlindPubsubImpl>();
				break;

			case ECloudProvider::AWS:
				UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector in AWS mode"));

				// Yes, this will cause trouble if more than one module links against
				// the AWS SDK but at this point I'll have to deal with that later
				if (!s_aws_sdk_initialized) {
					s_aws_sdk_options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
					Aws::InitAPI(s_aws_sdk_options);
					s_aws_sdk_initialized.Store(true);
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
					m_log_device = MakeUnique<FCloudWatchLogOutputDevice>(n_config->LogGroupPrefix);
					GLog->AddOutputDevice(m_log_device.Get());
				}

				m_storage = MakeUnique<AWSStorageImpl>();
				m_pubsub  = MakeUnique<AWSPubsubImpl>(n_config->HandleOnGameThread);
				break;

			case ECloudProvider::GOOGLE:
				UE_LOG(LogCloudConnector, Display, TEXT("Starting Cloud Connector in Google mode"));
				m_storage = MakeUnique<GoogleCloudStorageImpl>();
				m_pubsub  = MakeUnique<GooglePubsubImpl>(n_config->GoogleProjectId, n_config->HandleOnGameThread);
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

				// I am not deleting this object as the engine should call TearDown()
				// upon it to release resources
				if (s_aws_sdk_initialized) {
					Aws::Utils::Logging::ShutdownAWSLogging();

					// Again, this will cause trouble if more than one module links against the AWS SDK.
					// I am well aware of this
					Aws::ShutdownAPI(s_aws_sdk_options);
					s_aws_sdk_initialized.Store(false);
				}

				break;

			case ECloudProvider::GOOGLE:
				UE_LOG(LogCloudConnector, Display, TEXT("Stopping Google Cloud Connector"));
				break;
		}

		m_provider = ECloudProvider::BLIND;
		//m_storage.Reset();
		//m_pubsub.Reset();
	}
}

ICloudStorage &FCloudConnectorModule::storage() const {

	checkf(m_storage, TEXT("You are calling this too early, please wait for the game to start"))
	return *m_storage;
}

ICloudPubsub &FCloudConnectorModule::pubsub() const {

	checkf(m_pubsub, TEXT("You are calling this too early or too late, please wait for the game to start"))
	return *m_pubsub;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FCloudConnectorModule, CloudConnector)

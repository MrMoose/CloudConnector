/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudConnectorModule.h"
#include "CloudConnector.h"
#include "AWSStorageImpl.h"
#include "AWSPubsubImpl.h"
#include "GoogleCloudStorageImpl.h"
#include "GooglePubsubImpl.h"

DEFINE_LOG_CATEGORY(LogCloudConnector)
#define LOCTEXT_NAMESPACE "FCloudConnectorModule"

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
			case ECloudProvider::AWS:
				m_storage = MakeUnique<AWSStorageImpl>();
				m_pubsub  = MakeUnique<AWSPubsubImpl>();
			case ECloudProvider::GOOGLE:
				m_storage = MakeUnique<GoogleCloudStorageImpl>();
				m_pubsub  = MakeUnique<GooglePubsubImpl>();
		}
	} else {
		m_provider = ECloudProvider::BLIND;
		m_storage.Reset();
		m_pubsub.Reset();
	}
}

ICloudStorage &FCloudConnectorModule::storage() const {

	checkf(m_storage, TEXT("You are calling this too early, please wait for the game to start"))
	return *m_storage;
}

ICloudPubsub &FCloudConnectorModule::pubsub() const {

	checkf(m_pubsub, TEXT("You are calling this too early, please wait for the game to start"))
	return *m_pubsub;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FCloudConnectorModule, CloudConnector)

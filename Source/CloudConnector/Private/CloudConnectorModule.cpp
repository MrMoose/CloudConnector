/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudConnectorModule.h"

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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FCloudConnectorModule, CloudConnector)

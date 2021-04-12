/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudConnector.h"
#include "Misc/OutputDevice.h"

/// Implementation module for Cloud connectivity
/// Totally experimental work in progress. Do not use.
class FCloudConnectorModule : public ICloudConnector {

	public:
		/// IModuleInterface interface
		void StartupModule() override;
		void ShutdownModule() override;
		bool SupportsDynamicReloading() override;

		ICloudStorage &storage() const override;
		ICloudPubsub &pubsub() const override;

	private:

		void init_actor_config(const class ACloudConnector *n_config) override;

		ECloudProvider m_provider = ECloudProvider::AWS;

		TUniquePtr<ICloudStorage>  m_storage;
		TUniquePtr<ICloudPubsub>   m_pubsub;
		TUniquePtr<FOutputDevice>  m_log_device;
};

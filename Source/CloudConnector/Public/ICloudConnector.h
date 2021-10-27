/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "CloudConnectorTypes.h"
#include "ICloudTracing.h"
#include "ICloudStorage.h"
#include "ICloudQueue.h"
#include "ICloudPubsub.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogCloudConnector, Log, All);

/** Main interface to CloudConnector.
 *  Use getters storage(), pubsub() and tracing() for specific
 *  functionalities
 */
class CLOUDCONNECTOR_API ICloudConnector : public IModuleInterface {

	public:
		/// Virtual destructor.
		virtual ~ICloudConnector() = default;

		/// Get a reference to the module instance.
		static ICloudConnector &Get() {
			static const FName ModuleName = "CloudConnector";
			return FModuleManager::LoadModuleChecked<ICloudConnector>(ModuleName);
		}

		virtual ICloudStorage &storage() const = 0;
		virtual ICloudQueue   &queue() const = 0;
		virtual ICloudPubsub  &pubsub() const = 0;
		virtual ICloudTracing &tracing() const = 0;

	private:
		friend class ACloudConnector;
		virtual void init_actor_config(const class ACloudConnector *n_config) = 0;
};


/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCloudConnector, Log, All);

class CLOUDCONNECTOR_API ICloudConnector : public IModuleInterface {

	public:

		/// Virtual destructor.
		virtual ~ICloudConnector() = default;

		/// Get a reference to the module instance.
		static ICloudConnector &Get() {
			static const FName ModuleName = "CloudConnector";
			return FModuleManager::LoadModuleChecked<ICloudConnector>(ModuleName);
		}
};


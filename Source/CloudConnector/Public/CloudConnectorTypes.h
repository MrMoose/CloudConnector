/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"

#include "CloudConnectorTypes.generated.h"


/** @brief Well, we gotta start somewhere.
 *  So far it's AWS only but I really intend to add at least one more.
 */
UENUM(Category = "CloudConnector")
enum class ECloudProvider : uint8 {
	/// not implemented yet but this is supposed to become a "blind" impl for local tests withouut any cloud
	BLIND  = 0,

	/// Jeff Bezos' relentless little company
	AWS    = 1,

	/// Yet another evil empire
	GOOGLE = 2
};

/** @brief Unreal's internal log verbosity type cannot be used as a Property
 *  so I need to copy it here in order to expose this value on ACloudConnector.
 *  Not recommended to do so. See LogVerbosity.h for description
 */
UENUM(Category = "CloudConnector")
enum class  ECloudLogVerbosityLimit : uint8 {

	NoLogging = 0,
	Fatal,
	Error,
	Warning,
	Display,
	Log,
	Verbose,
	All
};
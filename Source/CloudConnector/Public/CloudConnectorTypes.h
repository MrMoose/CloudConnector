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


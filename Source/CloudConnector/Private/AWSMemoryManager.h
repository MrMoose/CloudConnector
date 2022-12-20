/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudTracing.h"

#include "Windows/PreWindowsApi.h"
#include "aws/core/utils/memory/MemorySystemInterface.h"
#include "Windows/PostWindowsApi.h"

/** An AWS Memory manager that maps to Unreal's
 */
class AWSMemoryManager : public Aws::Utils::Memory::MemorySystemInterface {
	
	public:
		void Begin() override;
		void *AllocateMemory(std::size_t n_size, std::size_t n_alignment, const char *allocationTag = nullptr) override;
		void FreeMemory(void *n_block) override;
		void End() override;
};

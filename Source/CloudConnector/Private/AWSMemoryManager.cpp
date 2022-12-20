/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "AWSMemoryManager.h"
#include "ICloudConnector.h"

#include "HAL/UnrealMemory.h"

void AWSMemoryManager::Begin() {

}

void *AWSMemoryManager::AllocateMemory(std::size_t n_size, std::size_t n_alignment, const char *allocationTag /* = nullptr */) {

	return FMemory::Malloc(n_size, n_alignment);
}

void AWSMemoryManager::FreeMemory(void *n_block) {

	FMemory::Free(n_block);
}

void AWSMemoryManager::End() {

}

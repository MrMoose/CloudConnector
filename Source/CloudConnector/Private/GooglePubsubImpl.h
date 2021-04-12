/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudPubsub.h"

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

 // Google Cloud SDK
#include "Windows/PreWindowsApi.h"
#include "google/cloud/completion_queue.h"
#include "Windows/PostWindowsApi.h"

#pragma warning(pop)

#include <thread>
#include <vector>

/** default IStorage implementation using files on local disk
 */
class GooglePubsubImpl : public ICloudPubsub {

	public:
		GooglePubsubImpl(const FString &n_project_id);
		~GooglePubsubImpl() noexcept;

		bool subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) override;

	private:
		const FString                  m_project_id;
		google::cloud::CompletionQueue m_completion_q;
	//	std::vector<std::thread>       m_runners;
		TUniquePtr<FThread>            m_runner;   //!< background thread for the SDK
};

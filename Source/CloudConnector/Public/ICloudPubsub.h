/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "Templates/UniquePtr.h"
#include "Templates/SharedPointer.h"
#include "Async/Future.h"

#include "ICloudPubsub.generated.h"

/** Subscription info.
 *  This is a handle to an existing subscription as handed out by ICloudPubsub::subscribe()
 */  
USTRUCT(Category = "CloudConnector")
struct CLOUDCONNECTOR_API FSubscription {

	GENERATED_BODY();

	/// Primary identifier, you may not need to use this explicitly
	UPROPERTY()
	FString Id;

	/// This is the topic name for Google Pubsub
	/// and the Q URL for AWS SQS
	UPROPERTY()
	FString Topic;

	/// These are needed to be able to use FSubscription as key to a TMap
	bool operator==(const FSubscription &n_other) const {

		return Equals(n_other);
	}

	bool Equals(const FSubscription &n_other) const {

		return Id.Equals(n_other.Id) || Topic.Equals(n_other.Topic);
	}
};

FORCEINLINE uint32 GetTypeHash(const FSubscription &n_sub) {

	return FCrc::MemCrc32(&n_sub, sizeof(FSubscription));
}

USTRUCT(Category = "CloudConnector")
struct FPubsubMessage {

	GENERATED_BODY();

	/// internally used
	FString m_message_id;

	/**
	 * use this to delete (acknowledge reception) the message
	 */
	FString m_receipt;

	/**
	 * age is in milliseconds
	 */
	uint32  m_message_age;

	/**
	 * is set when contained in the message response.
	 * This can be used as trace_id for start_trace_segment() and related functions
	 * to measure steps along the way of this message being processed
	 */
	FString m_xray_header;

	/**
	 * depending on the impl we may be able to tell how often the message has been received.
	 * 0 means unknown
	 */
	int m_receive_count = 0;

	/**
	 * message body
	 */
	FString m_body;

	private:

	friend class AWSPubsubImpl;
	
	/// Pubsub providers have to store implementation specific
	/// details such as message IDs and stuff along with the message.
	/// They are free to do this in here. Please do not mess with the
	/// contents of this field. 
	FString m_details;
};


/** conveys results of a write() operation
 *  first bool is "was the operation successful"?
 *  FString may contain an error message
 */
DECLARE_DELEGATE_TwoParams(FCloudStorageWriteFinishedDelegate, const bool, const FString);


using PubsubReturnPromise = TPromise<bool>;
using PubsubReturnPromisePtr = TSharedPtr<PubsubReturnPromise, ESPMode::ThreadSafe>;

/// First parameter is message body
/// Second parameter is a promise the delegate must fulfill. If it's set to true, the message will be deleted
DECLARE_DELEGATE_TwoParams(FPubsubMessageReceived, const FPubsubMessage, PubsubReturnPromisePtr);





/** Provide Pubsub equivalent functionality for AWS and Google Cloud.
 *  I'm trying to consolidate the two behind a common interface.
 *  Let's see how I roll.
 */
class CLOUDCONNECTOR_API ICloudPubsub {

	public:
		/// This maps to visibility timeout on SQS and ACK deadline on Pubsub
		enum { VisibilityTimeout = 30 };

		virtual ~ICloudPubsub() = default;

		/** @brief Subscribe to the default subscription as specified environment
		 *  variable CLOUDCONNECTOR_DEFAULT_TOPIC
		 *  Users are strongly encouraged to unsubscribe_default().
		 *  This is safe to be called from any thread.
		 *
		 *  @param n_subscription will hold the subscription handle (to unsubscribe)
		 *			if return is false, contents are undefined
		 *  @param n_completion will fire on the game thread (or in its own, depending on config) when the operation is complete
		 *  @return true when the operation was successfully started
		 */
		virtual bool subscribe_default(FSubscription &n_subscription, const FPubsubMessageReceived n_handler);

		/** @brief Subscribe to a "topic" which maps to a SQS Queue URL or a Pubsub topic
		 *  You can subscribe to multiple topics but only once to each. Meaning
		 *  Subscribing twice to the same topic results in undefined behavior.
		 *  Users are strongly encouraged to unsubscribe() from each topic.
		 *  This is safe to be called from any thread.
		 * 
		 *  @param n_topic the Queue URL (SQS) or Topic (Pubsub) you want to subscribe to
		 *  @param n_subscription will hold the subscription handle (to unsubscribe)
		 *			if return is false, contents are undefined
		 *  @param n_completion will fire on the game thread (or in its own, depending on config) when the operation is complete
		 *  @return true when the operation was successfully started
		 */
		virtual bool subscribe(const FString &n_topic, FSubscription &n_subscription, const FPubsubMessageReceived n_handler) = 0;
	
		/** @brief Unsubscribe from a subscription
		 *  Remaining messages may be in flight. This blocks until all remaining handlers
		 *  have been called.
		 *  This is safe to be called from any thread.
		 * 
		 *  @param n_subscription As given by subscribe(). Do not attempt to create this yourself.
		 *  
		 *  @return true when the operation was successful
		 */
		virtual bool unsubscribe(FSubscription &&n_subscription) = 0;
};

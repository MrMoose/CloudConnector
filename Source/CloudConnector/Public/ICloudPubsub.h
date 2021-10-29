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
	/// and the SNS Topic for AWS
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
struct CLOUDCONNECTOR_API FPubsubMessage {

	GENERATED_BODY();

	public:
		/**
		 * age is in milliseconds
		 */
		uint32  m_message_age;

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
#ifdef WITH_GOOGLECLOUD_SDK
		friend class GooglePubsubImpl;
#endif
		friend class AWSPubsubImpl;
		friend class SQSRunner;
		friend class SQSSubscription;

		/// Pubsub providers have to store implementation specific
		/// details such as message IDs and stuff along with the message.
		/// They are free to do this in here. Please do not mess with the
		/// contents of this field. 
		FString m_details;

		/// internally used by AWS
		FString m_aws_sqs_message_id;

		/// internally used by AWS
		FString m_aws_sqs_receipt;

		/// internally used by Google
		FString m_google_pubsub_message_id;
};

/** conveys results of a write() operation
 *  first bool is "was the operation successful"?
 *  FString may contain an error message
 */
DECLARE_DELEGATE_TwoParams(FCloudStorageWriteFinishedDelegate, const bool, const FString);

/// a promise created for each message which must be fulfilled by receivers
using PubsubReturnPromise = TPromise<bool>;
using PubsubReturnPromisePtr = TSharedPtr<PubsubReturnPromise, ESPMode::ThreadSafe>;

/// First parameter is message body
/// Second parameter is a promise the delegate must fulfill. If it's set to true, the message will be deleted
/// Third is a trace object. May be null if the received message did not contain trace info
DECLARE_DELEGATE_TwoParams(FPubsubMessageReceived, const FPubsubMessage, PubsubReturnPromisePtr);

/// First parameter is OK (true) or Error (false)
/// Second parameter is possible error message in case of false
///     or whatever the impl gives us as a message ID in case of true
DECLARE_DELEGATE_TwoParams(FPubsubMessagePublished, const bool, const FString);


/** Provide Pubsub equivalent functionality for AWS and Google Cloud.
 *  I'm trying to consolidate the two behind a common interface.
 *  Let's see how I roll.
 */
class CLOUDCONNECTOR_API ICloudPubsub {

	public:
		/// Maps to a timeout in secs of how long we wait for the SDK to send 
		/// a message before we call the handler with an error
		enum { MessageSendTimeout = 30 };

		virtual ~ICloudPubsub() noexcept = default;

		virtual void shutdown() noexcept = 0;

		/** @brief Publish a message to a topic
		 *  Message is sent converted to UTF8
		 *  You may or may not provide a handler. If the function returns true the
		 *  handler will always be called. If it returns false the handler will never be called.
		 *  Performance-wise it's better to only supply a handler if you need to know when
		 *  and if the message is guaranteed to be sent.
		 */
		virtual bool publish(const FString &n_topic, const FString &n_message, FPubsubMessagePublished &&n_handler = FPubsubMessagePublished{}) = 0;

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

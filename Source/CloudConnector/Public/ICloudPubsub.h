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

/** Sub info
 */  
USTRUCT(Category = "CloudConnector")
struct CLOUDCONNECTOR_API FSubscription {

	GENERATED_BODY();

	/**
	 */ 
	UPROPERTY()
	FString Topic;

};

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
	 * message body
	 */
	FString m_body;


	private:

	friend class AWSPubsubImpl;
	
	/// Pubsub providers have to store implementation specific
	/// details such as message IDs and stuff along with the message.
	/// They are free to do this in here. Please do not mess with the
	/// contents of this field. 
	UPROPERTY()
	FString Details;
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
		virtual ~ICloudPubsub() = default;

		/** @brief Make up this interface as I go
		 *  @param n_key the identifier for this object
		 *  @param n_completion will fire on the game thread when the operation is complete
		 *  @return true when the operation was successfully started, in which case the delegate will always fire
		 */
		virtual FSubscription subscribe(const FString &n_topic, const FPubsubMessageReceived n_handler) = 0;

};

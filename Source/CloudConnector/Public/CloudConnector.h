/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#pragma once

#include "CoreMinimal.h"
#include "ICloudConnector.h"
#include "CloudConnectorTypes.h"
#include "GameFramework/Actor.h"

/*
 Make absolutely sure to never include any private provider specific
 headers or types here!
 */

#include "CloudConnector.generated.h"


/**
 * Placing this Actor in your persistent level activates usage 
 * of Cloud Connectivity for Unreal Engine.
 * 
 * As this is experimental, a few notes:
 *    -> Do not spawn multiple actors of this type. Really, I'm serious.
 *    -> Do not use in production
 */
UCLASS(Category = "CloudConnector")
class CLOUDCONNECTOR_API ACloudConnector : public AActor {

	GENERATED_BODY()

	public:
		ACloudConnector();

		// Other actors may want to use this in their BeginPlay()
		// so I should register that a bit earlier
		void PostInitializeComponents() override;

		//void BeginPlay() override;
		void EndPlay(const EEndPlayReason::Type n_reason) override;

		/// Choose the cloud provider to use.
		/// Note that now only AWS is implemented
		UPROPERTY(EditAnywhere, Category = "CloudConnector")
		ECloudProvider CloudProvider = ECloudProvider::AWS;

		/** @brief Set to true to enable logs.
		 * All engine logs are sent to the appropriate logging provider, 
		 * CloudWatch in case of AWS.
		 * A log group called $LogGroupPrefix/$instance_id.
		 * The logs will be pushed every 5 seconds, which incurs costs 
		 * and CloudWatch is not cheap. Think before activating this.
		 * At startup time, you can override this setting with the environment 
		 * variable CLOUDCONNECTOR_LOGS_ENABLED to "True" or "False"
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|Logging")
		bool CloudLogs = false;
		
		/** @brief specify prefix for CloudWatch log group.
		 * CLOUDCONNECTOR_STACK_NAME and Instance ID will be read and appended.
		 * Please have it start and end with a '/' character.
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|Logging")
		FString LogGroupPrefix = TEXT("/cc/unreal/");

		/** @brief Set to true to make the AWS SDK log if used.
		 * Log files will be generated in the current directory the application
		 * (or editor) is started in and begin with aws_sdk_
		 * This shouldn't be required.
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|AWS")
		bool AWSLogs = false;

		/** @brief Set the Project scope on Google Cloud
		 *  AWS doesn't have an equivalent of the "Project" scope Google uses,
		 *  yet this is needed all over the place for Google.
		 *  I didn't want to expose this on the C++ interface so you have to set
		 *  this here when you use Google Cloud
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|GoogleCloud")
		FString GoogleProjectId = TEXT("CloudConnectorTest");
	
		/** @brief Set this to false if you want message handle delegates to be called on any thread.
		 *  Normally received message handlers will be called on the game thread.
		 *  If you do not require this or you want to stay in concurrency while processing
		 *  the message you can set this to false. In that case you cannot call into most
		 *  engine functionality in your handle delegate.
		 *  When in doubt, leave this true.
		 */
		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector|Pubsub")
		bool HandleOnGameThread = true;

	private:
#if WITH_EDITORONLY_DATA
		/// AWS icon sprite
		UPROPERTY()
		class UBillboardComponent *m_sprite_component;

		/// AWS icon texture
		UPROPERTY()
		class UTexture2D          *m_aws_icon_texture;
#endif
};

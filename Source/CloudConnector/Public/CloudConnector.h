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
 * A few notes:
 *    -> Do not spawn multiple actors of this type. Really, I'm serious.
 *    -> Use with care in production. This is a beta
 * 
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

		/** Set to true to enable logs.
		 * All engine logs are sent to the appropriate logging provider, 
		 * CloudWatch in case of AWS.
		 * A log group called $LogGroupPrefix/$instance_id.
		 * The logs will be pushed every 5 seconds, which incurs costs 
		 * and CloudWatch is not cheap. Think before activating this.
		 * At startup time, you can override this setting with the environment 
		 * variable CLOUDCONNECTOR_LOGS_ENABLED to "True" or "False"
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector")
		bool CloudLogs = false;

		/** Set to true to enable performance tracing.
		 * 
		 * At startup time, you can override this setting with the environment
		 * variable CLOUDCONNECTOR_TRACING_ENABLED to "True" or "False"
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector")
		bool Tracing = false;

		/** Specify prefix for CloudWatch log group.
		 * CLOUDCONNECTOR_STACK_NAME and Instance ID will be read and appended.
		 * Please have it start and end with a '/' character.
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|Logging")
		FString LogGroupPrefix = TEXT("/cc/unreal/");

		/** Set to true to make the AWS SDK log if used.
		 * Log files will be generated in the current directory the application
		 * (or editor) is started in and begin with aws_sdk_
		 * This shouldn't be required.
		 */
		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector")
		bool AWSLogs = false;
	
		/** In cases where you don't want the AWS SDK to take its credentials
		 *  from ~/.aws/credentials you can inject your access key and
		 *  secret key.
		 *  You should leave this empty unless you know exactly what you are doing.
		 *  If this is not empty, ~/.aws/credentials will be ignored.
		 *  You can override this at runtime using environment
		 *  variable CLOUDCONNECTOR_AWS_ACCESS_KEY and
		 *  CLOUDCONNECTOR_AWS_SECRET_KEY.
		 */
		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector")
		FString AWSAccessKey = FString{};

		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector")
		FString AWSSecretKey = FString{};

		/// Tell AWS to use a specific region. This is needed when setting
		/// AWS Access Key and secret key above.
		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector")
		FString AWSRegion = FString{};

		/// Tell AWS client objects to use the optional endpoint discovery feature
		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector")
		bool AWSEnableEndpointDiscovery = false;

		/// Requested feature. When set to true, CloudConnector will not log when PIE
		UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "CloudConnector")
		bool AWSSuppressCloudWatchLogsInPIE = false;

		/** Set the Project scope on Google Cloud
		 *  AWS doesn't have an equivalent of the "Project" scope Google uses,
		 *  yet this is needed all over the place for Google.
		 *  I didn't want to expose this on the C++ interface so you have to set
		 *  this here when you use Google Cloud
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|GoogleCloud")
		FString GoogleProjectId = TEXT("CloudConnectorTest");
	
		/** Set this to false if you want message handle delegates to be called on any thread.
		 *  Normally received message handlers will be called on the game thread.
		 *  If you do not require this or you want to stay in concurrency while processing
		 *  the message you can set this to false. In that case you cannot call into most
		 *  engine functionality in your handle delegate.
		 *  When in doubt, leave this true.
		 */
		UPROPERTY(EditAnywhere, Category = "CloudConnector|Pubsub")
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

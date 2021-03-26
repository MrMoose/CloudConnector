/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudConnector.h"
#include "ICloudConnector.h"
#include "Utilities.h"
#include "CloudWatchLogOutputDevice.h"

#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Templates/Atomic.h"

#include "Windows/PreWindowsApi.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include "Windows/PostWindowsApi.h"


TAtomic<bool>    s_aws_sdk_initialized = false;
Aws::SDKOptions  s_aws_sdk_options;


ACloudConnector::ACloudConnector()
		: AActor() {

	PrimaryActorTick.bCanEverTick = false;
	SetActorEnableCollision(false);
	bReplicates = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	RootComponent->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FObjectFinder<UTexture2D> aws_sprite(TEXT("/CloudConnector/Decals/AWSLogo.AWSLogo"));
	if (aws_sprite.Succeeded()) {
		m_aws_icon_texture = aws_sprite.Object;

		// This creates a little in-Editor icon for the config so one can see it in scene
#if WITH_EDITORONLY_DATA
		m_sprite_component = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpriteComponent"));
		m_sprite_component->SetMobility(EComponentMobility::Static);
		m_sprite_component->SetSprite(m_aws_icon_texture);
		m_sprite_component->SetupAttachment(RootComponent);
#endif
	}
}

// Called when the game starts or when spawned
void ACloudConnector::BeginPlay() {

	Super::BeginPlay();

	switch (CloudProvider) {
		default:
		case ECloudProvider::BLIND:
			begin_play_blind();
			break;

		case ECloudProvider::AWS:
			begin_play_aws();
			break;
	}
}

void ACloudConnector::EndPlay(const EEndPlayReason::Type n_reason) {

	switch (CloudProvider) {
		default:
		case ECloudProvider::BLIND:
			end_play_blind();
			break;

		case ECloudProvider::AWS:
			end_play_aws();
			break;
	}

	Super::EndPlay(n_reason);
}

void ACloudConnector::begin_play_blind() {

	UE_LOG(LogCloudConnector, Display, TEXT("Starting Blind Connector"));
}

void ACloudConnector::begin_play_aws() {

	UE_LOG(LogCloudConnector, Display, TEXT("Starting AWS Connector"));

	// Yes, this will cause trouble if more than one module links against
	// the AWS SDK but at this point I'll have to deal with that later
	if (!s_aws_sdk_initialized) {
		s_aws_sdk_options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
		Aws::InitAPI(s_aws_sdk_options);
		s_aws_sdk_initialized.Store(true);
	}

	if (AWSLogs) {
		// You won't need logging in live system. This is file IO after all.
		// Disable this for production unless you need it
		Aws::Utils::Logging::InitializeAWSLogging(
			Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(
				"CCAllocationTag", Aws::Utils::Logging::LogLevel::Info, "aws_sdk_"));
	}

	// If the user wants logs, an outputdevice is injected into the engine logs
	if (logs_enabled(CloudLogs)) {
		m_log_device = MakeUnique<FCloudWatchLogOutputDevice>(LogGroupPrefix);
		GLog->AddOutputDevice(m_log_device.Get());
	}
}

void ACloudConnector::end_play_blind() {
	
	UE_LOG(LogCloudConnector, Display, TEXT("Stopping Blind Connector"));
}

void ACloudConnector::end_play_aws() {

	if (m_log_device) {
		GLog->RemoveOutputDevice(m_log_device.Get());
		m_log_device.Reset();  // Calling d'tor and joining thread.
	}

	// I am not deleting this object as the engine should call TearDown()
	// upon it to release resources
	if (s_aws_sdk_initialized) {
		if (AWSLogs) {
			Aws::Utils::Logging::ShutdownAWSLogging();
		}

		// Again, this will cause trouble if more than one module links against the AWS SDK.
		// I am well aware of this
		Aws::ShutdownAPI(s_aws_sdk_options);
		s_aws_sdk_initialized.Store(false);
	}
}
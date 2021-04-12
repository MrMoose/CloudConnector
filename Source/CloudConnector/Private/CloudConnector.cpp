/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "CloudConnector.h"
#include "ICloudConnector.h"

#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Templates/Atomic.h"


ACloudConnector::ACloudConnector()
		: AActor() {

	PrimaryActorTick.bCanEverTick = false;
	SetActorEnableCollision(false);
	bReplicates = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	RootComponent->SetMobility(EComponentMobility::Static);

#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UTexture2D> aws_sprite(TEXT("/CloudConnector/Decals/CC_Logo.CC_Logo"));
	if (aws_sprite.Succeeded()) {
		m_aws_icon_texture = aws_sprite.Object;
	
		// This creates a little in-Editor billboard icon which incidentally looks like a butt.
		m_sprite_component = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpriteComponent"));
		m_sprite_component->SetMobility(EComponentMobility::Static);
		m_sprite_component->SetSprite(m_aws_icon_texture);
		m_sprite_component->SetEditorScale(0.25f);
		m_sprite_component->SetupAttachment(RootComponent);
	}
#endif
}

// Called when the game starts or when spawned
void ACloudConnector::BeginPlay() {

	Super::BeginPlay();
	ICloudConnector::Get().init_actor_config(this);
}

void ACloudConnector::EndPlay(const EEndPlayReason::Type n_reason) {

	ICloudConnector::Get().init_actor_config(nullptr);
	Super::EndPlay(n_reason);
}

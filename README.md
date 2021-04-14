# CloudConnector

#### Cloud connectivity for Unreal Engine

## About

CloudConnector is a plugin for the
[Unreal Engine](https://www.unrealengine.com/) which 
implements elementary cloud native connectivity functions
for use within C++ Unreal Engine projects.
Primary use case are remote rendering cases in industry contexts.
To this end, it focuses on logging, tracing and monitoring
as well as storage access capabilities and queue messages.
CloudConnector hides the actual cloud implementation behind
and abstracted common interface and aims for easy deployment
on AWS or Google Cloud with little or no code changes.

This is work in progress, there are no releases yet.

It only supports Win64 as of now. Other platforms are not planned. 
If you are interested in any other, 
contact [MrMoose](https://github.com/MrMoose).

## Usage

### Installation

Usage and installation are much like any other plugin:

Navigate to your project's "Plugins" folder or create it 
if not present.

```shell
 > cd d:\UnrealProjects\MyProject\Plugins
 > git clone --depth 0 https://github.com/MrMoose/CloudConnector.git
```

In your `build.cs` of your module, add CloudConnector to the private or public
dependencies.

```c#
PrivateDependencyModuleNames.AddRange(new string[] { 
		"CloudConnector"
});
```

After this, rebuild your Visual Studio solution and build your project.
Then start the editor.

### Environment

Some environment variables influence CloudConnector's behavior 
for ease of use on cloud instances. Some are cloud specific.

*General:*<br>

_CLOUDCONNECTOR_DEFAULT_TOPIC_:<br>

When using ICloudPubsub::subscribe_default(), this is the topic 
you will subscribe to.

_CLOUDCONNECTOR_LOGS_ENABLED_:<br>

Even when the configuration actor has Cloud Logging disabled, you can
override at runtime by setting this to `True`.

*AWS specific:*<br>

CLOUDCONNECTOR_ENDPOINT_DISCOVERY_ENABLED:<br>
If set to `True`, AWS client objects will be created with the 
"endpoint discovery" option enabled, which is opt-in.


### Activation

To activate this plugin, you need to create _one_ actor of type
`ACloudConnector` in your scene. Within the editor, navigate to 
the content folder of
`CloudConnector C++ Classes/CloudConnector/Public/` and drag 
the actor icon into your scene. Clicking on it allows for configuring 
properties in its "Details" window.

_Cloud Provider_:<br>
* Blind - Inactive dormant implementation. Will be for testing purposes
* AWS - Select this to use AWS
* Google Cloud - Select this to use Google Cloud

_Logging_:<br>
Various logging properties

_AWS_:<br>
AWS specific options that may or may not influence behavior in that impl.

_GoogleCloud_:<br>
Google cloud specific options that may or may not influence behavior in that impl.

Note that the presence of the actor is affecting functionality
during game runtime. It does not do anything during editor time.

### Development

During development you are likely to work on a local machine 
outside the cloud. To use CloudConnector's facilities, you will
need to provide credentials to your Unreal process so it can access
the cloud services.

On AWS this is often being done using `~/.aws/credentials`. 
[See here](https://docs.aws.amazon.com/general/latest/gr/aws-sec-cred-types.html)
for AWS documentation on this. It involves creating a 
key for programmatic access and then putting it into a file
called `~/.aws/credentials` in your user dir.

For Google Cloud the process is similar.
[Here is the documentation](https://cloud.google.com/docs/authentication/production).
You create a service account for programmatic access, get a key pair and 
store it in a local json file, where an env variable points to.

In both cases CloudConnector will use the credentials provided 
much like any other cloud tool would.

When running on a cloud instance, this is generally not necessary.
On AWS for example, a role will be attached to the instance 
and CloudConnector will honor this.

## Logging

Logging is one of CloudConnector's primary funtions. 
It hooks up to Unreal's log system and directs the entire log output 
to [AWS CloudWatch](https://aws.amazon.com/cloudwatch/) or 
[Google Cloud Logging](https://cloud.google.com/logging/docs/).
This works transparently.

When starting up, CC creates a log group with the name 
of your instance (or `LocalInstance`) when outside of the cloud
and a log stream with the current date.

Note that Google Cloud logging is not yet implemented, as there is
no SDK support for it yet. Once this is available, it will be added. 
Until then, it's only AWS CloudWatch.

## Storage

Storage capabilities map to [S3](https://aws.amazon.com/s3/) on
AWS and to [Storage](https://cloud.google.com/storage) on 
Google Cloud.

Usage is encapsulated by the `ICloudStorage` interface. Use like this:

```C++
#include "ICloudConnector.h"

...

ICloudStorage &storage = ICloudConnector::Get().storage();

// Get or fill your data buffer. You are responsible 
// for its lifetime.
TArray64<uint8> data;

// Create information about where to put your data
FCloudStorageKey key;
key.ObjectKey = TEXT("PictureOfMyCat.jpg");
key.BucketName = TEXT("super-cat-pic-bucket");
key.ContentType = TEXT("image/jpg");

// Create a non-owning view of your data
TArrayView<const uint8> view{ data };

// Trigger the write operation. Lambda will fire upon return
storage.write(key, view,
	FCloudStorageWriteFinishedDelegate::CreateLambda(
		[key](const bool n_success, const FString n_message) {
			if (n_success) {
				UE_LOG(LogRayStudio, Display, TEXT("'%s' uploaded: %s"), *key.ObjectKey, *n_message);
			} else {
				UE_LOG(LogRayStudio, Error, TEXT("'%s' not uploaded: %s"), *key.ObjectKey, *n_message);
			}
		}
	)
);

// If we would return here, our buffer would go out of scope
// during the operation, causing a crash or undefined behavior.
// So whatever happens here, data must not be deleted.

```

Note that the write operation happens asynchronously in 
impl-dependent fashion but is guaranteed to not block the calling
thread. The return delegate will fire in the game thread when the 
operation has finished. Multiple operations may happen at the same time.
The caller is responsible for maintaining ownership over the data 
buffer in that time. It must be kept alive until the delegate fires.
The delegate will only fire when `write()` returns `true`. 
If such is the case, it will always fire.

## Pubsub

This interface aims at mimicking a generic message retrieval system, 
making lots of concessions on the way. In any case, this is how you can use 
it.

First you need a function that will be called when a message comes in.
This can be any regular or member function, or a Lambda if you chose.
In this example I have an actor component that wants to subscribe to such messages.
Like this:

```C++
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ICloudConnector.h"

#include "QueueListener.generated.h"

UCLASS()
class UQueueListener : public UActorComponent {

	GENERATED_BODY()
	
	public:
		void BeginPlay() override;
		void EndPlay(const EEndPlayReason::Type n_reason) override;

	private:
		// we want this to be called for every incoming message
		void receive_message(const FPubsubMessage n_message, PubsubReturnPromisePtr n_promise);

		FSubscription m_subscription; // store this to unsubscribe
};
```

In our code, we can subscribe to a topic when the component begins play.
In this case an AWS SQS Url:

```C++
void UQueueListener::BeginPlay() {

	Super::BeginPlay();

	// Insert your Queue URL
	const FString topic = TEXT("https://sqs.eu-central-1.amazonaws.com/your-account/your-queue");

	ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();
	pubsub.subscribe(topic, m_subscription,
		FPubsubMessageReceived::CreateUObject(this, &UQueueListener::receive_message));
}
```

If result is `true` the connection was established and our method will be called when
messages are received. Here's a null implementation:

```C++
void UQueueListener::receive_message(const FPubsubMessage n_message, PubsubReturnPromisePtr n_retval) {

	UE_LOG(MyLog, Display, TEXT("Received message '%s'"), *n_message.m_body);
	n_promise->SetValue(false);
}
```

It is crucial that every call to this handler will at some point acknowledge 
the message by calling `SetValue()` on the return promise.

If this is set to true, the implementation will acknowledge (or delete) the message
so other listeners will not receive it anymore. If you set it to false, it will 
not be acknowledged and may be received again. In SQS case that is after the 
visibiliy timeout expires. In Pubsub it may mean right away.

Before the return promise is called, no other handlers are invoked and 
message retrieval pauses. You may freely leave the scope of your message handler
and go async at will, as long as you maintain ownership over the return promise.
`SetValue()` may be called from any thread.

By using the configuration actor's property `HandleOnGameThread` it is possible 
to control if your handler is called on the game thread or on the receiving 
component's own threads (which are all engine threads). By setting the property 
to false, the handler is called immediately when a message is received but you may
not be able to call into most engine functionality. Use with care.
When your application is done receiving messages, you must unsubscribe:

```C++
void UQueueListener::EndPlay(const EEndPlayReason::Type n_reason) {

	ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();
	pubsub.unsubscribe(MoveTemp(m_subscription));

	Super::EndPlay(n_reason);
}
```

After this, no further handlers will be called. Those still in flight 
will be handled.

## License

Copyright 2021 by Stephan Menzel

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

### AWS C++ SDK

This work uses the [AWS C++ SDK](https://github.com/aws/aws-sdk-cpp) 
in binary form. It is distributed under the Apache 2.0 license. 
[See here](https://github.com/aws/aws-sdk-cpp/blob/master/LICENSE) 
for full license agreement.

### Google Cloud C++ SDK

This work uses the [Google Cloud C++ SDK](https://github.com/googleapis/google-cloud-cpp)
in binary form. It is distributed under the Apache 2.0 license. 
[See here](https://github.com/googleapis/google-cloud-cpp/blob/master/LICENSE) 
for full license agreement.
Google Cloud C++ SDK has further, indirect, dependencies which are
described [here](https://github.com/googleapis/google-cloud-cpp/blob/master/doc/packaging.md).

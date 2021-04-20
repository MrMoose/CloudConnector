# CloudConnector

#### Cloud connectivity for Unreal Engine

## About

CloudConnector is a plugin for the
[Unreal Engine](https://www.unrealengine.com/) which 
implements elementary cloud native connectivity functions
for use within C++ Unreal Engine projects.
Primary use case are remote rendering cases in industry contexts.
To this end, it focuses on logging, tracing and monitoring
as well as storage access capabilities and message retrieval.
CloudConnector hides the actual cloud implementation behind
an abstracted common interface and aims for easy deployment
on AWS or Google Cloud with little or no code changes.

Using it you can:
* Receive messages from SQS or Pubsub
* Upload data into buckets on S3 or Storage
* Log to CloudWatch
* Write performance traces to XRay or Tracing (only XRay implemented now)

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

#### General

_CLOUDCONNECTOR_DEFAULT_TOPIC_:<br>
When using ICloudPubsub::subscribe_default(), this is the topic 
you will subscribe to.

_CLOUDCONNECTOR_LOGS_ENABLED_:<br>
Even when the configuration actor has Cloud Logging disabled, you can
override at runtime by setting this to `True`.

_CLOUDCONNECTOR_TRACING_ENABLED_:<br>
Even when the configuration actor has Cloud Tracing disabled, you can
override at runtime by setting this to `True`.

_CLOUDCONNECTOR_STACK_NAME_:<br>
This environment variable is taken whenever it comes to identifying a stack.
Normally this should always be injected by your IaC system.
When not set, it defaults to `UnknownStack`.

_CLOUDCONNECTOR_INSTANCE_ID_:<br>
CloudConnector will query the cloud provider's metadata server for the local
instance ID. For example, when creating log streams. This behavior can 
be overridden by setting this environment variable to a custom instance id.
Please note that this may lead to unsatisfactory results when using characters
that are not allowed in either log stream names or in pubsub subscriptions.
Use something simple that only contains "0-9a-zA-Z-".
If this is not set and a query to the metadata server is unsuccessful
(for example because you're not running in the cloud), it will
default to `LocalInstance`.

#### AWS specific 

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
If you do not have this yet, it is likely that you also need a
`config` file in the same directory with the region you would 
like to use.

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
specified in the property `LogGroupPrefix` of your configuration
actor. This will be appended by the value of the environment variable
`CLOUDCONNECTOR_STACK_NAME` if set, or "UnknownStack" otherwise.
So for a CloudWatch example, a log group named

`/cc/unreal/MyTestStack`

will be created. If it already exists, it will be used.

Each running instance of your application will then create a log stream with the
date and time of start plus the instance ID as a name.
`LocalInstance` when outside of the cloud.

Note that Google Cloud logging is not yet implemented, as there is
no SDK support for it yet. Once this is available, it will be added. 
Until then, it's only AWS CloudWatch.

!! *WARNING* !!<br>
A distributed Unreal engine has no logging in Shipping builds.
Meaning you won't see any logs out of CloudConnector in Shipping packages.
To enable logs in Shipping builds, you must build the engine from source and set:
```C#
bUseLoggingInShipping = true;
BuildEnvironment = TargetBuildEnvironment.Unique;
```
... in your Target.cs.


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
				UE_LOG(MyLog, Display, TEXT("'%s' uploaded: %s"), *key.ObjectKey, *n_message);
			} else {
				UE_LOG(MyLog, Error, TEXT("'%s' not uploaded: %s"), *key.ObjectKey, *n_message);
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

### AWS SQS Setting

When using AWS with CloudConnector your Queue needs to be configured for 
Long Polling with "Receive message wait time" of 4 seconds. CloudConnector
expects this setting to be the case. Short polling is not supported and 
longer wait times are not recommended.

## Tracing

Performance tracing is an essential aspect of CloudConnector. It aims at making
this easy, but of course it's not always that as tracing is a complex subject.

In most cases the plugin assumes there is sokme kind of tracing ID or similar coming
in from your cloud environment. This will normally be generated by your cloud 
environment. For example by a Lambda layer that creates your render job or the API
gateway. Going into detail is out of scope of this document. Let's just say you have
a trace within your cloud system and want your Unreal code using CloudConnector 
to appear as part of it.

Central core is an object of the class `CloudTrace`. It is threadsafe and you keep 
it around until you are done tracing. For the actual information you open 
segments upon it and close them when you are done. Times are tracked.

If you are using `IPubsub` to retrieve messages, and your message already contains
such a trace information, the `FPubsubMessage` object will already contain 
a `CloudTrace` ready. If not, you can create one using `ICloudTracing::start_trace()`._
Let's go back to our previous Q retrieve example.

```C++

#include "ICloudConnector.h"

void UQueueListener::receive_message(const FPubsubMessage n_message, PubsubReturnPromisePtr n_retval) {

	UE_LOG(MyLog, Display, TEXT("Received message '%s'"), *n_message.m_body);

	// See if we have a tracer already in our message
	CloudTracePtr tracer = n_message.m_trace;

	// If we do, we can use it to trace operations
	if (tracer) {
		tracer->start_segment(TEXT("longer running operation"));
	}

	// Do something noteworthy

	if (tracer) {
		tracer->end_segment(TEXT("longer running operation"));
	}

	n_promise->SetValue(true);
}
```

When your tracer goes out of scope (the shared pointer has no references anymore),
it will write the trace information to AWS XRay or Google Cloud Trace.

If you are the reckless daredevil, you can also use macros. They remove the need 
of the tedious `if` statements.

```C++
#include "ICloudConnector.h"
#include "TraceMacros.h"

void UQueueListener::receive_message(const FPubsubMessage n_message, PubsubReturnPromisePtr n_retval) {

	UE_LOG(MyLog, Display, TEXT("Received message '%s'"), *n_message.m_body);

	{
		CC_SCOPED_TRACE_SEGMENT(n_message.m_trace, TEXT("longer running operation"));
		// Do something heroic
	}

	// or not scoped:
	CC_START_TRACE_SEGMENT(n_trace, TEXT("another op"));
	// Do something phenomenal
	CC_END_TRACE_SEGMENT_OK(n_trace, TEXT("another op"));

	n_promise->SetValue(true);
}
```

When using `ICloudStorage`, you can pass the tracer object into each
to have storage implicitly trace. Just pass the object into the call.

If your trace ID comes in by other means than SQS/Pubsub, you can explicitly 
create a trace object like this:

```C++
#include "ICloudConnector.h"

CloudTracePtr tracer = ICloudConnector::Get().tracing().start_trace(your_trace_id);
```

Keep in mind though, the trace will be written when the `tracer` pointer goes out 
of scope. Not before.

### AWS XRay

If you are using AWS XRay there are a few things worth mentioning:

There may be need for the standalone [AWS XRay daemon](https://docs.aws.amazon.com/xray/latest/devguide/xray-daemon.html) on your local machine.

If you are in an isolated subnet (one without internet access), XRay will fail.
There are no VPC endpoints for it like for most other services and therefore 
you cannot trace unless your instance can reach the internet. However,
AWS may have remedied the situation by the time you read this, so better double check.


## Troubleshooting

_I have configured logging but see no logs in the CloudWatch console_:

You may have been missing a `config` file in `~/.aws/`. Try this:
```
[default]
region = eu-central-1
output = json
```
... but of course insert the correct region you want to use. Also 
see if your console browser is pointing into that region. Lastly 
note that logging is unavailable in shipping build configuration.

_I have configured logging but see no logs in the Google Logging console_:

Google logging is not yet implemented. Also 
note that logging is unavailable in shipping build configuration.

_I want to trace with XRay but no traces appear_:

If in an isolated subnet, XRay will not work due to the lack of VPC endpoints for it.
If not, check the following:
* is your infrastructure providing trace information? Is, for example, XRay active in your lambda? 
* are you creating a tracer object?
* are you opening and closing segments

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

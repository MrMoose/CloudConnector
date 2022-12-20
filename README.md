# CloudConnector

#### Cloud connectivity for Unreal Engine

## About

CloudConnector is a plugin for the
[Unreal Engine](https://www.unrealengine.com/) which 
implements elementary cloud native connectivity functions
for use within C++ Unreal Engine 4 projects.
Primary use case are cloud based remote rendering 
applications in industry contexts.
It focuses on queue message handling logging, tracing and monitoring
as well as storage access capabilities.
CloudConnector hides the actual cloud implementation behind
an abstracted common interface and aims for easy deployment
on AWS or Google Cloud with little or no code changes.

Using it you can:
* Receive queue from SQS or Google Pubsub
* Send and receive messages from SNS/SQS or Google Pubsub
* Upload data into buckets on S3 or Google Storage
* Log to CloudWatch or Google Logging
* Write performance traces to XRay or OpenTelemetry

It only supports Win64 as of now. Other platforms are not available 
right now, however Linux is scheduled to drop next year if possible. 
If you are interested in any other, 
contact [MrMoose](https://github.com/MrMoose).

Supported Engine versions:
* 4.26
* 4.27

Unreal Engine 5 might work. Users reported having done this but
it is not tested right now.

## Quickstart

This section gives a quick overview but omits lots of details. It's only an appetizer. 
Look in the sections below for full documentation.

First install the plugin by cloning the repo into your Unreal 
project's `Plugins` folder and build the Editor.

Open the Editor, create an actor of type `ACloudConnector` in your persistent level.

You should already have logging if you activate it in the actor's properties.<br>
To receive messages from SQS, implement a function like this

```C++
void receive_message(const FQueueMessage n_message, QueueReturnPromisePtr n_promise);
```

... then start the listening process by:

```C++
ICloudConnector::Get().queue().listen(queue_url_, m_subscription, delegate_bound_to_your_handler);
```

Upload files to S3 or Storage by:

```C++
// Key is like bucket + filename + mime type
FCloudStorageKey key{
	TEXT("PictureOfMyCat.jpg"),
	TEXT("super-cat-pic-bucket"),
	TEXT("image/jpg")
};

// a non-owning view of your data
TArrayView<const uint8> view{ data };

// Trigger the write operation. Lambda will fire upon return
ICloudConnector::Get().storage().write(key, view, FCloudStorageWriteFinishedDelegate::CreateLambda(...));
```

Interested? I thought so. Please see below for full documentation. If you find anything lacking, feel 
free to start a discussion.

## Usage

### Installation

Usage and installation are much like any other plugin:

Navigate to your project's "Plugins" folder or create it 
if not present.

```shell
 > cd d:\UnrealProjects\MyProject\Plugins
 > git clone --depth 1 https://github.com/MrMoose/CloudConnector.git
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
for ease of use on cloud instances. Some are cloud specific. See section 
"Environment Variables" below for full details.

*__Attention:__*
For usage of the plugin on Google Cloud it is essential to download and 
set Google gRPC Root Certificates
[as described here in section "Windows".](https://github.com/googleapis/google-cloud-cpp/blob/main/google/cloud/bigtable/quickstart/README.md)
Basically, make sure to download https://pki.google.com/roots.pem and set
environment variable GRPC_DEFAULT_SSL_ROOTS_FILE_PATH to the resulting file.

### Activation

To activate this plugin, you need to create _one_ actor of type
`ACloudConnector` in your scene. Within the editor, navigate to 
the content folder of
`CloudConnector C++ Classes/CloudConnector/Public/` and drag 
the actor icon into your scene. Clicking on it allows for configuring 
properties in its "Details" window.

_Cloud Provider_:<br>
* Blind - Inactive dormant implementation. For testing purposes. Does nothing.
* AWS - Select this to use AWS
* Google Cloud - Select this to use Google Cloud

_Logging_:<br>
Various logging properties

_AWS_:<br>
AWS specific options that may or may not influence behavior in that impl.

_GoogleCloud_:<br>
Google cloud specific options that may or may not influence behavior in that impl.

Note that the presence of the actor is affecting functionality
during game runtime. It does not do anything during editor time. It does work PIE though.

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

When starting up, CC creates a AWS log group with the name 
specified in the property `LogGroupPrefix` of your configuration
actor. This will be appended by the value of the environment variable
`CLOUDCONNECTOR_STACK_NAME` if set, or "UnknownStack" otherwise.
So for a CloudWatch example, a log group named

`/cc/unreal/MyTestStack`

will be created. If it already exists, it will be used.

Each running instance of your application will then create a log stream with the
date and time of start plus the instance ID as a name.
`LocalInstance` when outside of the cloud.

In Google Cloud the log name has a predefined structure and depends on the property
`GoogleProjectId` in the config actor.

The log name will be:

`projects/$YOUR_PROJECT_ID/logs/$UTC-$INSTANCE_ID_`


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

## Queue

There are two message related interfaces. The first is `ICloudQueue` which is 
aimed at simple job retrieval via queues. It maps to SQS on AWS and Google Tasks 
at Google, although due to lackof support for Tasks in Google's SDK it currently 
implements this using Pubsub.

To use it, you can implement a handler that is being called when a Q message 
comes in like this:

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
		void receive_message(const FQueueMessage n_message, QueueReturnPromisePtr n_promise);

		FQueueSubscription m_subscription; // store this to unsubscribe
};
```

We can then subscribe to a topic when the component begins play.
In this case an AWS SQS Url:

```C++
void UQueueListener::BeginPlay() {

	Super::BeginPlay();

	// Insert your Queue URL
	const FString topic = TEXT("https://sqs.eu-central-1.amazonaws.com/your-account/your-queue");

	ICloudQueue &queue = ICloudConnector::Get().queue();
	queue.listen(topic, m_subscription,
		FQueueMessageReceived::CreateUObject(this, &UQueueListener::receive_message));
}
```

You can stop listening on the Q using `stop_listen()` or `shutdown()`.

If result is `true` the connection was established and our method will be called when
messages are received. Here's a null implementation:

```C++
void UQueueListener::receive_message(const FQueueMessage n_message, QueueReturnPromisePtr n_retval) {

	UE_LOG(MyLog, Display, TEXT("Received message from q: '%s'"), *n_message.m_body);
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
message retrieval pauses. You may leave the scope of your message handler
and go async at will, as long as you maintain ownership over the return promise.
`SetValue()` may be called from any thread.

By using the configuration actor's property `HandleOnGameThread` it is possible 
to control if your handler is called on the game thread or on the receiving 
component's own threads (which are all engine threads). By setting the property 
to false, the handler is called immediately when a message is received but you may
not be able to call into most engine functionality. Use with care.
When your application is done receiving messages, you must unsubscribe:

### Queue Permissions

In order to use this service, your cloud role needs provider specific permissions.
Depending on AWS or Google they look different but generally involve permission 
to Receive Messages from a Queue, Delete them and Send ACK/NACK if appropriate. 
For Google (Pubsub) you must also supply permissions to create a Subscription as 
each instance will create and delete a subscription for itself.

### AWS SQS Setting

When using AWS with CloudConnector your Queue needs to be configured for 
Long Polling with "Receive message wait time" of 4 seconds. CloudConnector
expects this setting to be the case. Short polling is not supported and 
longer wait times are not recommended.

## Pubsub

### Subscribe to a topic and handle messages

This interface aims at mimicking a generic message exchange system. It is a little more
complex than `ICloudQueue` but follows a very similar pattern. On AWS this is implemented 
using a SNS / SQS combo. On Google it is Pubsub.

First you need a function that will be called when a message comes in.
This can be any regular or member function, or a Lambda if you chose.
In this example I have an actor component that wants to subscribe to such messages.
Like this:

```C++
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ICloudConnector.h"

#include "PubsubListener.generated.h"

UCLASS()
class UPubsubListener : public UActorComponent {

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
In this case an AWS SNS Topic:

```C++
void UPubsubListener::BeginPlay() {

	Super::BeginPlay();

	// Insert your Topic
	const FString topic = TEXT("jobs_topic");

	// In your subscription, set the Id to the Q URL
	m_subscription.Id = TEXT("https://sqs.eu-central-1.amazonaws.com/your-account/your-queue");
	
	// Set this to true if you don't want CloudConnector to create the subscription
	// but instead prefer to pre-create it in your infrastructure.
	m_subscription.Reused = true;

	ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();
	pubsub.subscribe(topic, m_subscription,
		FPubsubMessageReceived::CreateUObject(this, &UQueueListener::receive_message));
}
```

See the description of "Queue" above on how to treat the supplied Return future.

For Google Cloud (the actual Pub/Sub) this may look a bit different


```C++
void UPubsubListener::BeginPlay() {

	Super::BeginPlay();

	// Insert your Topic
	const FString topic = TEXT("jobs_topic");

	// There is no Q URL in Google but the actual name of the subscription
	m_subscription.Id = TEXT("jobs-subscription");
	
	// Set this to true if you don't want CloudConnector to create the subscription
	// but instead prefer to pre-create it in your infrastructure.
	m_subscription.Reused = true;

	ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();
	pubsub.subscribe(topic, m_subscription,
		FPubsubMessageReceived::CreateUObject(this, &UQueueListener::receive_message));
}
```


When your application is done receiving messages, you must unsubscribe:

```C++
void UPubsubListener::EndPlay(const EEndPlayReason::Type n_reason) {

	ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();
	pubsub.unsubscribe(MoveTemp(m_subscription));

	Super::EndPlay(n_reason);
}
```

After this, no further handlers will be called. Those still in flight 
will be handled.

### Publish a message to a topic

Note that sending a message to a topic is a purely asynchronous process which 
aims at returning immediately in order to not block the game thread.
You do not have to be subscribed to a topic you send a message to.
If the topic does not exist, implementations will try to create it with default values.
To send a message to a topic, you may do this:

```C++
ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();

if (!pubsub.publish(TEXT("MyTopic"), TEXT("Hello World"),
		FPubsubMessagePublished::CreateLambda([](const bool n_success, const FString n_message) {
			
	if (n_success) {
		UE_LOG(MyLog, Display, TEXT("Published message: '%s'"), *n_message);
	} else {
		UE_LOG(MyLog, Warning, TEXT("Failed to publish message: '%s'"), *n_message);
	}
}))) {	
	UE_LOG(MyLog, Warning, TEXT("Publish command was not accepted"));
}
```

Note that when sending messages, you do not have to supply a return handler. 
Only do so when you need confirmation that the message has been sent. Otherwise
leave it empty. Like this:

```C++
ICloudPubsub &pubsub = ICloudConnector::Get().pubsub();

if (!pubsub.publish(TEXT("MyTopic"), TEXT("Hello World"))) {
	UE_LOG(MyLog, Warning, TEXT("Publish command was not accepted"));
}
```

### Pubsub Permissions

Depending on AWS or Google you need the following permissions on your role:

* Create SNS or Pubsub Topics
* Create SQS Queues or Pubsub subscriptions
* Send and receive messages
* Delete and ACK messages
* Delete Subscriptions or Queues

CloudConnector will create a SQS queue to subscribe to an SNS topic on AWS 
and a subscription on Google if those do not exist already.
Should your application crash or lose its 
connection this subscription (or the Q) may not be removed and be left over. 
Users are advised to keep an eye on unused Queues and delete them manually if 
appropriate.

### AWS SNS

When using AWS SNS make sure your subscription has the
[raw message delivery flag](https://docs.aws.amazon.com/sns/latest/dg/sns-large-payload-raw-message-delivery.html)
set. CloudConnector assumes this to be the case. Otherwise your received body will
be wrapped into SNS metadata.

## Tracing

Performance tracing is an essential aspect of CloudConnector. It aims at making
this easy, but of course it's not always that as tracing is a complex subject.

In most cases the plugin assumes there is some kind of tracing ID or similar coming
in from your cloud environment. This will normally be generated by your cloud 
environment. For example by a Lambda layer that creates your render job or the API
gateway. Going into detail is out of scope of this document. Let's just say you have
a trace within your cloud system and want your Unreal code using CloudConnector 
to appear as part of it.

Central core is an object of the class `ICloudTrace`. It is threadsafe and you keep 
it around until you are done tracing. For the actual information you open 
segments upon it and close them when you are done. Times are tracked.

If you are using `IQueue` to retrieve messages, and your message already contains
such a trace information, the `FQueueMessage` object will already contain 
a `ICloudTrace` ready. If not, you can create one using `ICloudTracing::start_trace()`._
Let's go back to our previous Q retrieve example.

```C++

#include "ICloudConnector.h"

void UQueueListener::receive_message(const FQueueMessage n_message, QueueReturnPromisePtr n_retval) {

	UE_LOG(MyLog, Display, TEXT("Received message '%s'"), *n_message.m_body);

	// See if we have a tracer already in our message
	ICloudTracePtr tracer = n_message.m_trace;

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
it will write the trace information to AWS XRay or OpenTelemetry.

If you are the reckless daredevil, you can also use macros. They remove the need 
for the tedious `if` statements.

```C++
#include "ICloudConnector.h"
#include "TraceMacros.h"

void UQueueListener::receive_message(const FQueueMessage n_message, QueueReturnPromisePtr n_retval) {

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

When using `ICloudStorage`, you can pass the tracer object into each call
to have storage implicitly trace.

If your trace ID comes in by other means than SQS/Pubsub, you can explicitly 
create a trace object like this:

```C++
#include "ICloudConnector.h"

ICloudTracePtr tracer = ICloudConnector::Get().tracing().start_trace(your_trace_id);
```

Keep in mind though, the trace will be written when the `tracer` pointer goes out 
of scope. Not before.

### AWS XRay

If you are using AWS XRay there may be need for the standalone [AWS XRay daemon](https://docs.aws.amazon.com/xray/latest/devguide/xray-daemon.html) on your local machine.

A previously existing restriction about using XRay in isolated subnets being impossible due to
no XRay VPC Endpoints being available is no longer relevant. [There are now XRay VPC 
endpoints available](https://aws.amazon.com/about-aws/whats-new/2021/05/aws-x-ray-now-supports-vpc-endpoints/).
You just need to create one.

### OpenTelemetry

When using Google Cloud, the experimental [OpenTelemetry](https://opentelemetry.io) implementation of tracing is 
assumed. This means you will need additional infrastructure to see your traces.

An example would be the [Collector](https://opentelemetry.io/docs/collector/) daemon, which you 
may locally run in docker.

This is an example configuration to run it against [Zipkin](https://zipkin.io):

```Yaml
receivers:
  otlp:
    protocols:
      grpc:
        endpoint: 0.0.0.0:4317
      http:
        endpoint: 0.0.0.0:4318 

processors:
  batch:

exporters:
  zipkin:
    endpoint: "http://zipkin:9411/api/v2/spans"
```

## Environment Variables

In most scenarios stack specific configuration of running Unreal instances is best 
achieved using environment variables. You should prefer this over setting defaults in
the properties section of the config actor.

### General

_CLOUDCONNECTOR_CLOUD_PROVIDER_:<br>
Using this you can override the cloud provider setting in
the configuration actor to use a different impl. Values are:
* "None" - for none. Won't do anything
* "AWS"
* "Google"
All other values are ignored.

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
instance ID. For example, when creating log streams or subscriptions. This behavior can 
be overridden by setting this environment variable to a custom instance id.
Please note that this may lead to unsatisfactory results when using characters
that are not allowed in either log stream names or in pubsub subscriptions.
Use something simple that only contains "0-9a-zA-Z-".
If this is not set and a query to the metadata server is unsuccessful
(for example because you're not running in the cloud), it will
default to `LocalInstance`.

### AWS specific 

CLOUDCONNECTOR_ENDPOINT_DISCOVERY_ENABLED:<br>
If set to `True`, AWS client objects will be created with the 
"endpoint discovery" option enabled, which is opt-in.

CLOUDCONNECTOR_AWS_ACCESS_KEY, CLOUDCONNECTOR_AWS_SECRET_KEY:<br>
When both are set to a programmatic's users's keypair, these credentials are used
instead of the `~/.aws/credentials` mechanism.
It is not recommended to use this option. If you do, you should set
`CLOUDCONNECTOR_ENDPOINT_DISCOVERY_ENABLED` and `CLOUDCONNECTOR_AWS_REGION` as well

CLOUDCONNECTOR_AWS_REGION:<br>
If set, this region will be used when instantiating client objects.
All services will use that region.
It is not recommended to use this option.

CLOUDCONNECTOR_AWS_CLOUDWATCH_ENDPOINT,<br>
CLOUDCONNECTOR_AWS_SQS_ENDPOINT,<br>
CLOUDCONNECTOR_AWS_S3_ENDPOINT,<br>
CLOUDCONNECTOR_AWS_XRAY_ENDPOINT:<br>
If set, the SDK's automatic endpoint chosing mechanics will be
overridden with the appropriate value.
It is not recommended to use this option.

### Google Cloud Specific

_CLOUDCONNECTOR_OTLP_HTTP_ENDPPOINT_:<br>
When using OpenTelemetry tracing, this is the HTTP endpoint of you collector daemon. 
If this is set, the HTTP protocol will be used, otherwise gRPC.
An example would be `http://127.0.0.1:4318/v1/traces` if you run the daemon locally.
Any certificate TLS voodoo is not yet supported.

_CLOUDCONNECTOR_OTLP_GRPC_ENDPPOINT_:<br>
When using OpenTelemetry tracing, this is the GRPC endpoint of you collector daemon. 
This defaults to whatever the OpenTelemetry lib thinks is best. And so should you to be honest.

### Dependencies

CloudConnector contains all its dependencies in binary form. This may seem 
unusual outside of the game engine world but Unreal users expect a Plugin to be self
contained. So all the libraries and DLLs are part of the source tree.

If you do not want to link and execute some binaries downloaded from a less
than trustworthy dude in the internet, you can use [a script]("/tree/master/Scripts")
to build them yourself.


## Troubleshooting

#### On Google Cloud I get timeouts talking to any service

Make sure your have downloaded https://pki.google.com/roots.pem and set
environment variable GRPC_DEFAULT_SSL_ROOTS_FILE_PATH to the resulting file.
Failute to do so results in "Retry Limit Exceeded" timeouts.

#### On AWS I have configured logging but see no logs in the CloudWatch console

You may have been missing a `config` file in `~/.aws/`. Try this:
```
[default]
region = eu-central-1
output = json
```
... but of course insert the correct region you want to use. Also 
see if your console browser is pointing into that region. Lastly 
note that logging is unavailable in shipping build configuration.

#### I have configured logging but see no logs in my provider's Logging console

Logging is unavailable in shipping build configuration.
Perhaps you are using Shipping Configuration? If so, note that logging is not supported.

#### When trying to build a package, I get weird protobuf related linker errors

Check if you use the PixelStreaming plugin. It can not coexist with CloudConnector.
At least it can't in 4.27. See here:
See here: https://github.com/MrMoose/CloudConnector/issues/2.
This is a problem that occurrs when linking both together. If you need PixelStreaming,
you can avoid the problem by setting `SupportGoogleCloud` to `false` in
`CloudConnector.build.cs`. In which case you will be able to build but no Google
Cloud Support will be available.

## License

Copyright 2022 by Stephan Menzel

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

### OpenTelemetry SDK

This work uses the [OpenTelemetry SDK](https://github.com/open-telemetry/opentelemetry-cpp)
in binary form. It is distributed under the Apache 2.0 license. 
[See here](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/LICENSE) 
for full license agreement.

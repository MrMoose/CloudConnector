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

After this, rebuild your Visual Studio solution and build your project.
Then start the editor.

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

/* (c) 2021 by Stephan Menzel
 * Licensed under the Apache License, Version 2.0.
 * See attached file LICENSE for full details
 */
#include "GooglePubsubImpl.h"
#include "ICloudConnector.h"

#include "Async/Async.h"
#include "Internationalization/Regex.h"

 // GoogleCloud SDK uses a few Cpp features that cause problems in Unreal Context.
 // See here: https://github.com/akrzemi1/Optional/issues/57 for an explanation

#pragma warning(push)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

 // AWS SDK
#include "Windows/PreWindowsApi.h"

#include "google/cloud/pubsub/subscriber.h"

#include "Windows/PostWindowsApi.h"

// std
#include <iostream>
#include <memory>
#include <strstream>
#include <fstream>

namespace gc = google::cloud;
namespace pubsub = google::cloud::pubsub;

FSubscription GooglePubsubImpl::subscribe(const FString &n_topic, const FPubsubMessageReceived n_handler) {


    

    auto sample = [](pubsub::Subscriber subscriber) {
        return subscriber.Subscribe(
            [&](pubsub::Message const &m, pubsub::AckHandler h) {
                std::cout << "Received message " << m << "\n";
                std::move(h).ack();
                
            });
    };

    return {};
}

#pragma warning(pop)

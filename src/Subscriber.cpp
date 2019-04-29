/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "Subscriber.hpp"
#include "Conversion.hpp"

#include <soss/Message.hpp>

#include <functional>
#include <iostream>

namespace soss {
namespace fiware {

Subscriber::Subscriber(
        Connector* connector,
        const std::string& topic_name,
        const std::string& message_type,
        TopicSubscriberSystem::SubscriptionCallback soss_callback)

    : fiware_connector_(connector)
    , topic_name_{topic_name}
    , message_type_{message_type}
    , subscription_id_{}
    , soss_callback_{soss_callback}
{
}

Subscriber::~Subscriber()
{
    unsubscribe();
}

bool Subscriber::subscribe()
{
    using namespace std::placeholders;

    subscription_id_ = fiware_connector_->register_subscription(topic_name_, std::bind(&Subscriber::receive, this, _1));
    return !subscription_id_.empty();
}

bool Subscriber::unsubscribe()
{
    return fiware_connector_->unregister_subscription(subscription_id_);
}

void Subscriber::receive(const Json& fiware_message)
{
    std::cout << "[soss-fiware][subscriber]: translate message: fiware -> soss "
        << "(" << topic_name_ << ") "
        << std::endl;

    soss::Message soss_message = Conversion::fiware_to_soss(message_type_, fiware_message);

    soss_callback_(soss_message);
}


} // namespace fiware
} // namespace soss

/*
 * Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include "NGSIV2Connector.hpp"

#include <soss/Message.hpp>

#include <functional>
#include <iostream>

namespace soss {
namespace fiware {

Subscriber::Subscriber(
        NGSIV2Connector* fiware_connector,
        const std::string& topic_name,
        const std::string& message_type,
        TopicSubscriberSystem::SubscriptionCallback soss_callback)

    : fiware_connector_(fiware_connector)
    , topic_name_{topic_name}
    , message_type_{message_type}
    , subscription_id_{}
    , soss_callback_{soss_callback}
{
}

Subscriber::~Subscriber()
{
    fiware_connector_->unregister_subscription(subscription_id_);
}

bool Subscriber::subscribe()
{
    using namespace std::placeholders;

    subscription_id_ = fiware_connector_->register_subscription(
                topic_name_, message_type_, std::bind(&Subscriber::receive, this, _1));
    return !subscription_id_.empty();
}

void Subscriber::receive(const Json& fiware_message)
{
    std::cout << "[soss-fiware][subscriber]: translate message: fiware -> soss "
        "(" << topic_name_ << ") " << std::endl;

    soss::Message soss_message = Conversion::fiware_to_soss(message_type_, fiware_message);

    if(!soss_message.type.empty())
    {
        soss_callback_(soss_message);
    }
}


} // namespace fiware
} // namespace soss

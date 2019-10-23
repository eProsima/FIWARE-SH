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

#ifndef SOSS__FIWARE__INTERNAL__SUBSCRIBER_HPP
#define SOSS__FIWARE__INTERNAL__SUBSCRIBER_HPP

#include "conversion.hpp"
#include "NGSIV2Connector.hpp"

#include <soss/SystemHandle.hpp>

#include <functional>
#include <iostream>

namespace soss {
namespace fiware {

class Subscriber
{
public:
    Subscriber(
            NGSIV2Connector& fiware_connector,
            const std::string& topic_name,
            const xtypes::DynamicType& message_type,
            TopicSubscriberSystem::SubscriptionCallback soss_callback)
        : fiware_connector_(fiware_connector)
        , topic_name_{topic_name}
        , message_type_{message_type}
        , soss_callback_{soss_callback}
    {}

    virtual ~Subscriber()
    {
        fiware_connector_.unregister_subscription(subscription_id_);
    }

    bool subscribe()
    {
        using namespace std::placeholders;

        subscription_id_ = fiware_connector_.register_subscription(
            topic_name_,
            message_type_.name(),
            std::bind(&Subscriber::receive, this, _1));

        return !subscription_id_.empty();
    }

    void receive(
            const Json& fiware_message)
    {
        std::cout << "[soss-fiware][subscriber]: translate message: fiware -> soss "
            "(" << topic_name_ << ") " << std::endl;

        xtypes::DynamicData soss_message(message_type_);
        if(!conversion::fiware_to_soss(fiware_message, soss_message))
        {
            std::cerr << "[soss-fiware][subscriber]: conversion error" << std::endl;
            return;
        }

        soss_callback_(soss_message);
    }

private:
    NGSIV2Connector& fiware_connector_;

    std::string topic_name_;
    const xtypes::DynamicType& message_type_;

    std::string subscription_id_;

    TopicSubscriberSystem::SubscriptionCallback soss_callback_;
};


} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__SUBSCRIBER_HPP

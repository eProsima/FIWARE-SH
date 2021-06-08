/*
 * Copyright 2019  - present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef _IS_SH_FIWARE__INTERNAL__SUBSCRIBER_HPP_
#define _IS_SH_FIWARE__INTERNAL__SUBSCRIBER_HPP_

#include "conversion.hpp"
#include "NGSIV2Connector.hpp"

#include <is/systemhandle/SystemHandle.hpp>

#include <is/utils/Log.hpp>

#include <functional>
#include <iostream>

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {

// TODO doxygen
class Subscriber
{
public:

    Subscriber(
            NGSIV2Connector& fiware_connector,
            const std::string& topic_name,
            const xtypes::DynamicType& message_type,
            TopicSubscriberSystem::SubscriptionCallback* is_callback)
        : fiware_connector_(fiware_connector)
        , topic_name_(topic_name)
        , message_type_(message_type)
        , is_callback_(is_callback)
        , logger_("is::sh::FIWARE::Subscriber")
    {
    }

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

        if (subscription_id_.empty())
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Subscribing to topic '" << topic_name_ << "' with type '"
                    << message_type_.name() << "' failed" << std::endl;
        }
        else
        {
            logger_ << utils::Logger::Level::DEBUG
                    << "Subscribed to topic '" << topic_name_ << "' with type '"
                    << message_type_.name() << "'" << std::endl;
        }

        return !subscription_id_.empty();
    }

    void receive(
            const Json& fiware_message)
    {
        try
        {
            xtypes::DynamicData xtypes_message = json_xtypes::convert(message_type_, fiware_message, "value");

            logger_ << utils::Logger::Level::INFO
                    << "Translate message from FIWARE to Integration Service for topic '"
                    << topic_name_ << "' with type '" << message_type_.name()
                    << "', payload: [[ " << xtypes_message << " ]]" << std::endl;

            (*is_callback_)(xtypes_message, nullptr);
        }
        catch (const json_xtypes::UnsupportedType& e)
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Failed to convert message from FIWARE to Integration Service for topic '"
                    << topic_name_ << "' with type '" << message_type_.name()
                    << "'; reason: " << e.what() << std::endl;
        }
    }

private:

    NGSIV2Connector& fiware_connector_;

    std::string topic_name_;
    const xtypes::DynamicType& message_type_;

    std::string subscription_id_;

    TopicSubscriberSystem::SubscriptionCallback* is_callback_;

    utils::Logger logger_;
};

} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

#endif //  _IS_SH_FIWARE__INTERNAL__SUBSCRIBER_HPP

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

#ifndef SOSS__FIWARE__INTERNAL__SUBSCRIBER_HPP
#define SOSS__FIWARE__INTERNAL__SUBSCRIBER_HPP

#include "json/json.hpp"

#include <soss/SystemHandle.hpp>

namespace soss {
namespace fiware {


using Json = nlohmann::json;

class NGSIV2Connector;

class Subscriber
{
public:
    Subscriber(
            NGSIV2Connector* fiware_connector,
            const std::string& topic_name,
            const std::string& message_type,
            TopicSubscriberSystem::SubscriptionCallback soss_callback);

    ~Subscriber();

    Subscriber(const Subscriber& rhs) = delete;
    Subscriber& operator = (const Subscriber& rhs) = delete;
    Subscriber(Subscriber&& rhs) = delete;
    Subscriber& operator = (Subscriber&& rhs) = delete;

    bool subscribe();

    void receive(const Json& message);

private:
    NGSIV2Connector* const fiware_connector_;

    const std::string topic_name_;
    const std::string message_type_;

    std::string subscription_id_;

    TopicSubscriberSystem::SubscriptionCallback soss_callback_;
};


} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__SUBSCRIBER_HPP

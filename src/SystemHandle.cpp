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

#include "SystemHandle.hpp"

#include <soss/Mix.hpp>
#include <soss/Search.hpp>

#include <iostream>
#include <thread>

namespace soss {
namespace fiware{

bool SystemHandle::configure(
    const RequiredTypes& /* types */,
    const YAML::Node& configuration)
{
    if (!configuration["host"] ||
        !configuration["port"] ||
        !configuration["subscription_host"] ||
        !configuration["subscription_port"])
    {
        std::cerr << "[soss-fiware]: Configuration must have the host and port and the subscription port." << std::endl;
        return false;
    }

    std::string host = configuration["host"].as<std::string>();
    uint16_t port = configuration["port"].as<uint16_t>();
    std::string subscription_host = configuration["subscription_host"].as<std::string>();
    uint16_t subscription_port = configuration["subscription_port"].as<uint16_t>();

    fiware_connector_ = std::unique_ptr<Connector>(new Connector(host, port, subscription_host, subscription_port));

    std::cout << "[soss-fiware]: Configured!" << std::endl;
    return true;
}

bool SystemHandle::okay() const
{
    return !fiware_connector_->has_errors();
}

bool SystemHandle::spin_once()
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
    return okay();
}

bool SystemHandle::subscribe(
    const std::string& topic_name,
    const std::string& message_type,
    SubscriptionCallback callback,
    const YAML::Node& /*configuration*/)
{
    auto subscriber = std::make_shared<Subscriber>(fiware_connector_.get(), topic_name, message_type, callback);

    if (!subscriber->subscribe())
    {
        std::cerr << "[soss-fiware]: Error when subscribing to Fiware. "
            << "Topic: " << topic_name << ". "
            << "Type: " << message_type << ". "
            << std::endl;

        return false;
    }

    subscribers_.push_back(std::move(subscriber));

    std::cout << "[soss-fiware]: Subscriber created. "
        << "Topic: " << topic_name << ". "
        << "Type: " << message_type << ". "
        << std::endl;

    return true;
}

std::shared_ptr<TopicPublisher> SystemHandle::advertise(
    const std::string& topic_name,
    const std::string& message_type,
    const YAML::Node& /*configuration*/)
{
    auto publisher = std::make_shared<Publisher>(fiware_connector_.get(), topic_name, message_type);

    publishers_.push_back(std::move(publisher));

    std::cout << "[soss-fiware]: Publisher created. "
        << "Topic: " << topic_name << ". "
        << "Type: " << message_type << ". "
        << std::endl;

    return publishers_.back();
}


} // namespace fiware
} // namespace soss

SOSS_REGISTER_SYSTEM("fiware", soss::fiware::SystemHandle)

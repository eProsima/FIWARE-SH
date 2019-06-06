/*
 * Copyright (C) 2019 Open Source Robotics Foundation
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
#include "NGSIV2Connector.hpp"

#include <iostream>
#include <thread>
#include <asio.hpp>

namespace soss {
namespace fiware{

namespace {

// This function patches the problem of FIWARE types, which do not admit '/' in their type name.
std::string transform_type(const std::string& message_type)
{
    std::string type = message_type;

    for(size_t i = type.find('/'); i != std::string::npos; i = type.find('/', i))
    {
        type.replace(i, 1, "__");
    }

    return type;
}

std::string my_local_ip_from(const std::string& host, uint16_t port)
{
    try
    {
        asio::io_service service;
        asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(host), port);
        asio::ip::tcp::socket socket(service);
        socket.connect(endpoint);
        return socket.local_endpoint().address().to_string();
    }
    catch (std::exception&)
    {
        return "";
    }
}

} //namespace

SystemHandle::~SystemHandle() = default;

bool SystemHandle::configure(
    const RequiredTypes& /* types */,
    const YAML::Node& configuration)
{
    if (!configuration["host"] ||
        !configuration["port"])
    {
        std::cerr << "[soss-fiware]: configuration must have the fiware context broker host and port." << std::endl;
        return false;
    }

    std::string host = configuration["host"].as<std::string>();
    uint16_t port = configuration["port"].as<uint16_t>();

    uint16_t subscription_port = 0;
    if (configuration["subscription_port"])
    {
        subscription_port = configuration["subscription_port"].as<uint16_t>();
    }

    std::string subscription_host;
    if (configuration["subscription_host"])
    {
        subscription_host = configuration["subscription_host"].as<std::string>();
    }
    else
    {
        subscription_host = my_local_ip_from(host, port);
        if (subscription_host.empty())
        {
            std::cerr << "[soss-fiware]: Error getting the local ip addres from "
                << host << ":" << port << std::endl;
            return false;
        }
    }

    fiware_connector_ = std::make_unique<NGSIV2Connector>(host, port, subscription_host, subscription_port);

    std::cout << "[soss-fiware]: configured!" << std::endl;
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
    auto subscriber = std::make_shared<Subscriber>(
        fiware_connector_.get(), topic_name, transform_type(message_type), callback);

    if (!subscriber->subscribe())
    {
        std::cerr << "[soss-fiware]: error when subscribing to fiware, "
            "topic: " << topic_name << ", "
            "type: " << message_type << std::endl;

        return false;
    }

    subscribers_.push_back(std::move(subscriber));

    std::cout << "[soss-fiware]: subscriber created, "
        "topic: " << topic_name << ", "
        "type: " << message_type << std::endl;

    return true;
}

std::shared_ptr<TopicPublisher> SystemHandle::advertise(
    const std::string& topic_name,
    const std::string& message_type,
    const YAML::Node& /*configuration*/)
{
    auto publisher = std::make_shared<Publisher>(fiware_connector_.get(), topic_name, transform_type(message_type));

    publishers_.push_back(std::move(publisher));

    std::cout << "[soss-fiware]: publisher created, "
        "topic: " << topic_name << ", "
        "type: " << message_type << std::endl;

    return publishers_.back();
}


} // namespace fiware
} // namespace soss

SOSS_REGISTER_SYSTEM("fiware", soss::fiware::SystemHandle)

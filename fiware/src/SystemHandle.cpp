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


#include "Publisher.hpp"
#include "Subscriber.hpp"
#include "NGSIV2Connector.hpp"

#include <soss/SystemHandle.hpp>

#include <iostream>
#include <thread>
#include <asio.hpp>
#include <vector>
#include <memory>

namespace soss {
namespace fiware{

namespace {

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

} //anonymous namespace



class SystemHandle : public virtual TopicSystem
{
public:
    SystemHandle() = default;
    virtual ~SystemHandle() = default;

    bool configure(
        const RequiredTypes& required_types,
        const YAML::Node& configuration,
        soss::TypeRegistry& type_registry) override
    {
        if (!configuration["host"] || !configuration["port"])
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
        std::map<std::string, Json> fiware_raw_types = fiware_connector_->request_types();
        for(auto&& it: fiware_raw_types)
        {
            if(required_types.messages.count(it.first))
            {
                xtypes::StructType soss_type(it.first);
                if(!conversion::fiware_type_to_soss_type(it.second, soss_type))
                {
                    std::cerr << "[soss-fiware]: conversion error with type [" << it.first << "]" << std::endl;
                }
                type_registry.emplace(it.first, std::move(soss_type));
            }
        }

        std::cout << "[soss-fiware]: configured!" << std::endl;
        return true;
    }

    bool okay() const
    {
        return !fiware_connector_->has_errors();
    }

    bool spin_once()
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        return okay();
    }

    bool subscribe(
        const std::string& topic_name,
        const xtypes::DynamicType& message_type,
        SubscriptionCallback callback,
        const YAML::Node& /*configuration*/)
    {
        auto subscriber = std::make_shared<Subscriber>(
            *fiware_connector_, topic_name, message_type, callback);

        if (!subscriber->subscribe())
        {
            std::cerr << "[soss-fiware]: error when subscribing to fiware, "
                "topic: " << topic_name << ", "
                "type: " << message_type.name() << std::endl;

            return false;
        }

        subscribers_.push_back(std::move(subscriber));

        std::cout << "[soss-fiware]: subscriber created, "
            "topic: " << topic_name << ", "
            "type: " << message_type.name() << std::endl;

        return true;
    }

    std::shared_ptr<TopicPublisher> advertise(
        const std::string& topic_name,
        const xtypes::DynamicType& message_type,
        const YAML::Node& /*configuration*/)
    {
        auto publisher = std::make_shared<Publisher>(*fiware_connector_, topic_name, message_type);

        publishers_.push_back(std::move(publisher));

        std::cout << "[soss-fiware]: publisher created, "
            "topic: " << topic_name << ", "
            "type: " << message_type.name() << std::endl;

        return publishers_.back();
    }

private:
    std::unique_ptr<NGSIV2Connector> fiware_connector_;
    std::vector<std::shared_ptr<Publisher>> publishers_;
    std::vector<std::shared_ptr<Subscriber>> subscribers_;
};



} // namespace fiware
} // namespace soss

SOSS_REGISTER_SYSTEM("fiware", soss::fiware::SystemHandle)

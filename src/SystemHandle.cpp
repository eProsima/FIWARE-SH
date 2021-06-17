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

#include <is/systemhandle/SystemHandle.hpp>

#include <is/utils/Log.hpp>

#include <iostream>
#include <thread>
#include <asio.hpp>
#include <vector>
#include <memory>

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {

namespace {

std::string my_local_ip_from(
        const std::string& host,
        uint16_t port)
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


// TODO doxygen

class SystemHandle : public virtual is::TopicSystem
{
public:

    SystemHandle()
        : logger_("is::sh::FIWARE")
    {
    }

    virtual ~SystemHandle() = default;

    bool configure(
            const core::RequiredTypes& required_types,
            const YAML::Node& configuration,
            TypeRegistry& type_registry) override
    {
        if (!configuration["host"] || !configuration["port"])
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Configuration must set the FIWARE Context Broker "
                    << "'host' and 'port' fields" << std::endl;

            return false;
        }

        const std::string host = configuration["host"].as<std::string>();
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
                logger_ << utils::Logger::Level::ERROR
                        << "Error getting the local IP address from "
                        << host << ":" << port << std::endl;
                return false;
            }
        }

        fiware_connector_ = std::make_unique<NGSIV2Connector>(host, port, subscription_host, subscription_port);

        if (fiware_connector_)
        {
            logger_ << utils::Logger::Level::INFO
                    << "Created NGSIv2 connector with IP " << host << ":" << port << std::endl;
        }
        else
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Failed to create NGSIv2 connector for IP" << host << ":" << port << std::endl;

            return false;
        }

        std::map<std::string, Json> fiware_raw_types = fiware_connector_->request_types();
        for (auto&& it: fiware_raw_types)
        {
            if (required_types.messages.count(it.first))
            {
                xtypes::StructType xtype(it.first);
                if (!conversion::fiware_type_to_xtypes(it.second, xtype))
                {
                    logger_ << utils::Logger::Level::ERROR
                            << "Conversion for type '" << it.first << "' failed" << std::endl;
                }
                type_registry.emplace(it.first, std::move(xtype));
            }
        }

        logger_ << utils::Logger::Level::INFO << "Configured successfully" << std::endl;
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
            SubscriptionCallback* callback,
            const YAML::Node& /*configuration*/) override final
    {
        auto subscriber = std::make_shared<Subscriber>(
            *fiware_connector_, topic_name, message_type, callback);

        bool subscribed = subscriber->subscribe();
        if (subscribed)
        {
            subscribers_.emplace_back(std::move(subscriber));
        }

        return subscribed;
    }

    bool is_internal_message(
            void* /*filter_handle*/) override final
    {
        return false;
    }

    std::shared_ptr<TopicPublisher> advertise(
            const std::string& topic_name,
            const xtypes::DynamicType& message_type,
            const YAML::Node& /*configuration*/) override final
    {
        auto publisher = std::make_shared<Publisher>(*fiware_connector_, topic_name, message_type);

        if (publisher)
        {
            logger_ << utils::Logger::Level::DEBUG
                    << "Created publisher for topic '" << topic_name << "' with type '"
                    << message_type.name() << "'" << std::endl;

            publishers_.emplace_back(std::move(publisher));
            return publishers_.back();
        }
        else
        {
            logger_ << utils::Logger::Level::ERROR
                    << "Publisher for topic '" << topic_name << "' with type '"
                    << message_type.name() << "' was not created" << std::endl;

            return nullptr;
        }
    }

private:

    std::unique_ptr<NGSIV2Connector> fiware_connector_;
    std::vector<std::shared_ptr<Publisher> > publishers_;
    std::vector<std::shared_ptr<Subscriber> > subscribers_;

    utils::Logger logger_;
};

} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

IS_REGISTER_SYSTEM("fiware", eprosima::is::sh::fiware::SystemHandle)

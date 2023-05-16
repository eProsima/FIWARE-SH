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

#include "NGSIV2Connector.hpp"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <iostream>
#include <sstream>

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {


NGSIV2Connector::NGSIV2Connector(
        const std::string& remote_host,
        uint16_t remote_port,
        const std::string& listener_host,
        uint16_t listener_port)

    : host_(remote_host)
    , port_(remote_port)
    , listener_host_(listener_host)
    , listener_port_(listener_port)
    , listener_(listener_port, std::bind(&NGSIV2Connector::receive, this, std::placeholders::_1))
    , subscription_callbacks_()
    , logger_("is::sh::FIWARE::NGSIV2Connector")
{
}

std::string NGSIV2Connector::register_subscription(
        const std::string& entity,
        const std::string& type,
        FiwareSubscriptionCallback callback)
{
    if (subscription_callbacks_.empty() && !listener_.is_running())
    {
        listener_port_ = listener_.run();
    }

    std::stringstream url;
    url << "http://" << listener_host_ << ":" << listener_port_;

    Json subscription_entity;
    subscription_entity["id"] = entity;
    subscription_entity["type"] = type;

    Json manifest;
    manifest["subject"]["entities"] = Json::array({ subscription_entity });
    manifest["notification"]["http"]["url"] = url.str();

    std::string response = request("POST", true, "subscriptions", manifest);

    std::string subscription_id;
    std::istringstream response_header(response);

    for (std::string line; std::getline(response_header, line); )
    {
        if (line.find("Location: /v2/subscriptions/") != std::string::npos)
        {
            subscription_id = line.substr(line.find_last_of("/") + 1);
            subscription_id.pop_back(); // Remove \r tail
            break;
        }
    }

    if (subscription_id.empty())
    {
        logger_ << utils::Logger::Level::ERROR
                << "Registering subscription for entity '" << entity
                << "' with type '" << type << "' failed; response: '" << response << "'" << std::endl;

        return "";
    }
    else
    {
        std::unique_lock<std::mutex> lock(subscription_mutex_);
        subscription_callbacks_[subscription_id] = callback;
        lock.unlock();

        logger_ << utils::Logger::Level::DEBUG
                << "Subscription for entity '" << entity << "' with type '" << type
                << "' registered with ID: " << subscription_id << std::endl;

        return subscription_id;
    }
}

bool NGSIV2Connector::unregister_subscription(
        const std::string& subscription_id)
{
    std::string response = request("DELETE", false, "subscriptions/" + subscription_id, Json{});

    if (!response.empty())
    {
        logger_ << utils::Logger::Level::ERROR
                << "Unregister subscription with ID " << subscription_id
                << "failed, error response: " << response << std::endl;

        return false;
    }
    else
    {

        std::unique_lock<std::mutex> lock(subscription_mutex_);
        subscription_callbacks_.erase(subscription_id);
        lock.unlock();

        logger_ << utils::Logger::Level::DEBUG
                << "Subscription with ID " << subscription_id
                << " unregistered successfully." << std::endl;

        if (subscription_callbacks_.empty() && listener_.is_running())
        {
            listener_.stop();
        }

        return true;
    }
}

bool NGSIV2Connector::update_entity(
        const std::string& entity,
        const std::string& type,
        const Json& json_message)
{
    std::string urn = "entities/" + entity + "/attrs?type=" + type;
    std::string response = request("PUT", false, urn, json_message);

    if (!response.empty())
    {
        logger_ << utils::Logger::Level::ERROR
                << "Update entity '" << entity << "' with type '" << type << "' failed, "
                << "response: " << response << std::endl;
    }
    else
    {
        logger_ << utils::Logger::Level::DEBUG
                << "Update entity '" << entity << "' with type '" << type
                << "': " << json_message << std::endl;
    }


    return response.empty();
}

bool NGSIV2Connector::create_entity(
        const std::string& entity,
        const std::string& type,
        const Json& json_message)
{
    std::string urn = "entities";

    Json create;
    create["id"] = entity;
    create["type"] = type;
    create["date"]["type"] = type;
    create.merge_patch(json_message);

    std::string response = request("POST", false, urn, create);

    if (!response.empty())
    {
        logger_ << utils::Logger::Level::ERROR
                << "Create entity '" << entity << "' with type '" << type << "' failed, "
                << "response: " << response << std::endl;
    }
    else
    {
        logger_ << utils::Logger::Level::DEBUG
                << "Create entity '" << entity << "' with type '" << type
                << "': " << json_message << std::endl;
    }


    return response.empty();
}

std::map<std::string, Json> NGSIV2Connector::request_types()
{
    std::string response = request("GET", true, "types?", Json{});
    Json request = Json::parse(response.c_str() + response.find('[')); //skip HTTP header

    std::map<std::string, Json> types;
    for (auto& type_info: request)
    {
        std::string name = type_info["type"].get<std::string>();
        Json type = type_info["attrs"];
        types.emplace(name, std::move(type));
    }

    return types;
}

std::string NGSIV2Connector::request(
        const std::string& method,
        bool response_header,
        const std::string& urn,
        const Json& json_message)
{
    try
    {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        std::stringstream url;
        url << host_ << ":" << port_ << "/v2/" << urn;
        request.setOpt(new curlpp::options::Url(url.str()));
        //request.setOpt(new curlpp::options::Verbose(true)); //Enable for debugging purposes

        if (method != "DELETE" && method != "GET")
        {
            std::list<std::string> header;
            header.push_back("Content-Type: application/json");
            request.setOpt(new curlpp::options::HttpHeader(header));
        }
        request.setOpt(new curlpp::options::Header(response_header));
        request.setOpt(new curlpp::options::CustomRequest(method));

        std::string payload;

        if (method != "DELETE" && method != "GET")
        {
            payload = json_message.dump();
            request.setOpt(new curlpp::options::PostFields(payload));
            request.setOpt(new curlpp::options::PostFieldSize(static_cast<long>(payload.length())));
        }

        std::stringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        logger_ << utils::Logger::Level::INFO
                << "Request to FIWARE, url: '" << url.str() << "', method: '" << method << "'";

        if ("DELETE" != method && "GET" != method)
        {
            logger_ << ", payload: '" << payload << "'";
        }
        logger_ << std::endl;

        return response.str();
    }
    catch (curlpp::LogicError& e)
    {
        logger_ << utils::Logger::Level::ERROR
                << "cURL logic error: " << e.what() << std::endl;
    }
    catch (curlpp::RuntimeError& e)
    {
        logger_ << utils::Logger::Level::ERROR
                << "cURL runtime error: " << e.what() << std::endl;
    }

    return "";
}

void NGSIV2Connector::receive(
        const std::string& message)
{
    Json json = Json::parse(message.c_str() + message.find('{')); //skip http header

    std::string subscription_id = json["subscriptionId"];

    logger_ << utils::Logger::Level::INFO
            << "Received message from subscription ID: " << subscription_id << " - ";

    FiwareSubscriptionCallback callback = nullptr;

    std::unique_lock<std::mutex> lock(subscription_mutex_);
    auto it = subscription_callbacks_.find(subscription_id);
    if (subscription_callbacks_.end() != it)
    {
        callback = it->second;
    }
    lock.unlock();

    if (callback != nullptr)
    {
        logger_ << "accepted. Data: [[ " << json << " ]]" << std::endl;
        callback(json);
    }
    else
    {
        logger_ << "skipping" << std::endl;
    }
}

} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

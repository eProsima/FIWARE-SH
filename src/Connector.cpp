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

#include "Connector.hpp"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <iostream>
#include <sstream>

namespace soss {
namespace fiware {


using namespace std::placeholders;

Connector::Connector(
        const std::string& remote_host,
        uint16_t remote_port,
        const std::string& listener_host,
        uint16_t listener_port)

    : host_{remote_host}
    , port_{remote_port}
    , listener_host_{listener_host}
    , listener_{listener_port, std::bind(&Connector::receive, this, _1)}
    , subscription_callbacks_{}
{
}

std::string Connector::register_subscription(
        const std::string& entity,
        FiwareSubscriptionCallback callback)
{
    if (subscription_callbacks_.empty() && !listener_.is_running())
    {
        listener_.run();
    }

    std::stringstream url;
    url << "http://" << listener_host_ << ":" << listener_.get_port();

    Json subscription_entity;
    subscription_entity["id"] = entity;

    Json manifest;
    manifest["subject"]["entities"] = { subscription_entity };
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
        std::cerr << "[soss-fiware][connector]: error registering subscription, "
                     "response: " << std::endl << response << std::endl;

        return "";
    }

    subscription_callbacks_[subscription_id] = callback;
    std::cout << "[soss-fiware][connector]: Subscription registered. "
                 "ID: " << subscription_id << std::endl;

    return subscription_id;
}

bool Connector::unregister_subscription(
        const std::string& subscription_id)
{
    std::string response = request("DELETE", false, "subscriptions/" + subscription_id, Json{});

    if (!response.empty())
    {
        std::cerr << "[soss-fiware][connector]: unsubscription error, "
                     "response: " << std::endl << response << std::endl;

        return false;
    }

    subscription_callbacks_.erase(subscription_id);

    std::cout <<  "[soss-fiware][connector]: Connector with ID " << subscription_id <<
                  " unregistered successfully." << std::endl;

    if (subscription_callbacks_.empty() && listener_.is_running())
    {
        listener_.stop();
    }

    return true;
}

bool Connector::update_entity(
        const std::string& entity,
        const Json& json_message)
{
    std::string response = request("PUT", false, "entities/" + entity + "/attrs", json_message);

    if (!response.empty())
    {
        std::cerr << "[soss-fiware][connector]: update entity error, "
                     "response: " << std::endl << response << std::endl;
    }

    return response.empty();
}

std::string Connector::request(
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
        //request.setOpt(new curlpp::options::Verbose(true));

        if (method != "DELETE")
        {
            std::list<std::string> header;
            header.push_back("Content-Type: application/json");
            request.setOpt(new curlpp::options::HttpHeader(header));
        }
        request.setOpt(new curlpp::options::Header(response_header));
        request.setOpt(new curlpp::options::CustomRequest(method));

        std::string payload;

        if (method != "DELETE")
        {
            payload = json_message.dump();
            request.setOpt(new curlpp::options::PostFields(payload));
            request.setOpt(new curlpp::options::PostFieldSize(static_cast<long>(payload.length())));
        }

        std::stringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        std::cout << "[soss-fiware][connector]: request to fiware, "
        << "url: " << url.str() << ", "
        << "method: " << method;
        if (method != "DELETE")
        {
            std::cout << ", payload: " << std::endl
            << "  " << payload;
        }
        std::cout << std::endl;

        return response.str();
    }
    catch (curlpp::LogicError& e)
    {
        std::cerr << "[soss-fiware][connector]: curl logic error: " << e.what() << std::endl;
    }
    catch (curlpp::RuntimeError& e)
    {
        std::cerr << "[soss-fiware][connector]: curl runtime error: " << e.what() << std::endl;
    }

    return "";
}

void Connector::receive(
            const std::string& message)
{
    Json json = Json::parse(message.c_str() + message.find('{'));

    std::string subscription_id = json["subscriptionId"];
    Json topic_data = json["data"][0];
    topic_data.erase("id");
    topic_data.erase("type");

    std::cout << "[soss-fiware][connector] received message from subscription ID: " << subscription_id << " - ";

    auto it = subscription_callbacks_.find(subscription_id);
    if (subscription_callbacks_.end() != it)
    {
        std::cout << "accepted" << std::endl;
        it->second(topic_data);
    }
    else
    {

        std::cout << "skipping" << std::endl;
    }
}


} //namespace fiware
} //namespace soss


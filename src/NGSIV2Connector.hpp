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

#ifndef SOSS__FIWARE__INTERNAL__NGSIV2CONNECTOR_HPP
#define SOSS__FIWARE__INTERNAL__NGSIV2CONNECTOR_HPP

#include "json/json.hpp"

#include "Listener.hpp"

#include <functional>
#include <map>

namespace soss {
namespace fiware {


using Json = nlohmann::json;

class NGSIV2Connector {

    using FiwareSubscriptionCallback = std::function<void(const Json& message)>;

public:
    NGSIV2Connector(
            const std::string& remote_host,
            uint16_t remote_port,
            const std::string& listener_host,
            uint16_t listener_port);

    virtual ~NGSIV2Connector() = default;

    std::string register_subscription(
            const std::string& entity,
            const std::string& type,
            FiwareSubscriptionCallback callback);

    bool unregister_subscription(
            const std::string& subscription_id);

    bool update_entity(
            const std::string& entity,
            const Json& json_message);

    const std::string& get_host() const { return host_; }
    uint16_t get_port() const { return port_; }
    const Listener& get_listener() const { return listener_; }

    bool has_errors() const { return listener_.has_errors(); }

private:
    std::string request(
            const std::string& method,
            bool response_header,
            const std::string& urn,
            const Json& json_message);

    void receive(
            const std::string& message);

    std::string host_;
    uint16_t port_;

    std::string listener_host_;
    Listener listener_;

    std::map<std::string, FiwareSubscriptionCallback> subscription_callbacks_;
};


} // namespace fiware
} // namespace soss

#endif // SOSS__FIWARE__INTERNAL__NGSIV2CONNECTOR_HPP

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

#include <soss/mock/api.hpp>
#include <soss/Instance.hpp>
#include <soss/utilities.hpp>

#include <catch2/catch.hpp>

#include <iostream>
#include <iomanip>
#include <ctime>

#define FIWARE_IP      FIWARE__IP__TEST_CONFIG
#define FIWARE_PORT    FIWARE__PORT__TEST_CONFIG
#define FIWARE_ADDRESS FIWARE_IP ":" FIWARE_PORT

std::string gen_config_yaml(
        const std::string& topic_type,
        const std::string& fiware_entity,
        const std::string& topic_sent,
        const std::string& topic_recv)
{
    std::string s;
    s += "systems:\n";
    s += "    fiware: { type: fiware, host: \"" FIWARE_IP "\", port: " FIWARE_PORT "}\n";
    s += "    mock: { type: mock }\n";

    s += "routes:\n";
    s += "    mock_to_fiware: { from: mock, to: fiware }\n";
    s += "    fiware_to_mock: { from: fiware, to: mock }\n";

    s += "topics:\n";
    s += "    " + topic_sent + ": { type: \"" + topic_type + "\", route: mock_to_fiware, \
                    remap: {fiware: \"" + fiware_entity + "\" } }\n";
    s += "    " + topic_recv + ": { type: \"" + topic_type + "\", route: fiware_to_mock, \
                    remap: {fiware: \"" + fiware_entity + "\" } }\n";
    return s;
}

soss::Message roundtrip(
        const std::string& fiware_entity,
        const std::string& topic_type,
        const soss::Message& message)
{
    using namespace std::chrono_literals;

    const std::string topic_sent = "mock_to_fiware_topic";
    const std::string topic_recv = "fiware_to_mock_topic";

    std::string config_yaml = gen_config_yaml(topic_type, fiware_entity, topic_sent, topic_recv);
    const YAML::Node config_node = YAML::Load(config_yaml);
    soss::InstanceHandle soss_handle = soss::run_instance(config_node);
    REQUIRE(soss_handle);

    std::this_thread::sleep_for(1s); //skip the first message from fiware, possibily from previous tests

    soss::mock::publish_message(topic_sent, message);

    std::promise<soss::Message> receive_msg_promise;
    REQUIRE(soss::mock::subscribe(
            topic_recv,
            [&](const soss::Message& msg_from_fiware)
    {
        receive_msg_promise.set_value(msg_from_fiware);
    }));

    auto receive_msg_future = receive_msg_promise.get_future();
    REQUIRE(std::future_status::ready == receive_msg_future.wait_for(5s));

    REQUIRE(0 == soss_handle.quit().wait_for(1s));
    return receive_msg_future.get();
}

bool create_fiware_entity(const std::string& entity, const std::string& type)
{
    const std::string create_entity_command =
        "curl " FIWARE_ADDRESS "/v2/entities -s -S -H \"Content-Type: application/json\" -d"
        "\"{"
        "    \\\"id\\\": \\\""+ entity + "\\\","
        "    \\\"type\\\": \\\"" + type + "\\\","
        "    \\\"data\\\": {"
        "        \\\"value\\\": \\\"\\\","
        "        \\\"type\\\": \\\"String\\\""
        "    }"
        "}\"";
    return 0 == system(create_entity_command.c_str());
}

bool delete_fiware_entity(const std::string& entity, const std::string& type)
{
    const std::string delete_entity_command =
        "curl " FIWARE_ADDRESS "/v2/entities/" + entity + "?type=" + type + " -s -S -H \
        \"Accept: application/json\" -X DELETE";

    return 0 == system(delete_entity_command.c_str());
}

TEST_CASE("Transmit to and receive from fiware", "[fiware]")
{
    SECTION("basic-type")
    {
        const std::string fiware_entity = "fiware_mock_test_basic";
        const std::string topic_type = "fiware_test_string";

        //Remove previous instance if exists
        delete_fiware_entity(fiware_entity, topic_type);

        REQUIRE(create_fiware_entity(fiware_entity, topic_type));

        SECTION("roundtrip")
        {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
            std::string message_data = "mock test message at " + ss.str();
            std::cout << "[test]: message id: " << ss.str() << std::endl;

            soss::Message msg_to_fiware;
            msg_to_fiware.type = topic_type;
            msg_to_fiware.data["data"] = soss::Convert<std::string>::make_soss_field(message_data);

            soss::Message msg_from_fiware = roundtrip(
                    fiware_entity,
                    topic_type,
                    msg_to_fiware);

            REQUIRE(message_data == *msg_from_fiware.data.at("data").cast<std::string>());
            REQUIRE(msg_to_fiware.type == msg_from_fiware.type);
        }

        REQUIRE(delete_fiware_entity(fiware_entity, topic_type));
    }
}

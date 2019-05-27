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

#define FIWARE_ADDRESS "192.168.1.59:1026" //TODO: read from yaml configuration

const char* fiware_create_test_entity_command =
    "curl " FIWARE_ADDRESS "/v2/entities -s -S -H 'Content-Type: application/json' -d @- <<EOF \n"
    "{\n"
    "    \"id\": \"fiware_mock_topic\",\n"
    "    \"type\": \"fiware_test_string\",\n"
    "    \"data\": {\n"
    "        \"value\": \"\",\n"
    "        \"type\": \"String\"\n"
    "    }\n"
    "}\n"
    "EOF";

const char* fiware_delete_test_entity_command =
    "curl " FIWARE_ADDRESS "/v2/entities/fiware_mock_topic?type=fiware_test_string -s -S -H \
    'Accept: application/json' -X DELETE";


TEST_CASE("Transmit to and receive from fiware", "[fiware]")
{
    using namespace std::chrono_literals;

    REQUIRE(0 == system(fiware_create_test_entity_command));

    SECTION("Publish and subscribe")
    {
        std::cout << "[test]: configuring middleware..." << std::endl;
        const YAML::Node config_node = YAML::LoadFile(FIWARE__ROUNDTRIP__TEST_CONFIG);
        soss::InstanceHandle soss_handle = soss::run_instance(config_node);
        REQUIRE(soss_handle);

        std::this_thread::sleep_for(1s); //skip the first message from fiware, possibily from previous tests
        std::cout << "[test]: preparing communication..." << std::endl;

        // Generating message
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        std::string message_data = "mock test message at " + ss.str();
        std::cout << "[test]: message id: " << ss.str() << std::endl;

        // Message to fiware
        soss::Message msg_to_fiware;
        msg_to_fiware.type = "fiware_test_string";
        msg_to_fiware.data["data"] = soss::Convert<std::string>::make_soss_field(message_data);
        soss::mock::publish_message("mock_to_fiware_topic", msg_to_fiware);

        // Message from fiware
        std::promise<soss::Message> receive_msg_promise;
        REQUIRE(soss::mock::subscribe(
                "fiware_to_mock_topic",
                [&](const soss::Message& msg_from_fiware)
        {
            receive_msg_promise.set_value(msg_from_fiware);
        }));

        auto receive_msg_future = receive_msg_promise.get_future();
        REQUIRE(std::future_status::ready == receive_msg_future.wait_for(5s));

        const soss::Message msg_from_fiware = receive_msg_future.get();
        REQUIRE(message_data == *msg_from_fiware.data.at("data").cast<std::string>());
        REQUIRE(msg_to_fiware.type == msg_from_fiware.type);

        std::cout << "[test]: Finishing commuinication..." << std::endl;
    }

    REQUIRE(0 == system(fiware_delete_test_entity_command));
}

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

#include <catch2/catch.hpp>

#include <iostream>


TEST_CASE("Transmit and receive all test messages", "[fiware]")
{
    using namespace std::chrono_literals;

    std::cout << "Variable: " << FIWARE__ROUNDTRIP__TEST_CONFIG << std::endl;

    const YAML::Node fiware_config_node = YAML::LoadFile(FIWARE__ROUNDTRIP__TEST_CONFIG);
    soss::InstanceHandle fiware_handle = soss::run_instance(fiware_config_node);
    REQUIRE(fiware_handle);

    std::this_thread::sleep_for(5s);
    std::cout << "Testing..." << std::endl;

    /*
    std::promise<soss::Message> to_mock_promise;
    auto client_to_server_future = client_to_server_promise.get_future();
    REQUIRE(soss::mock::subscribe("client_to_server",[&](const soss::Message& message)
    {
        client_to_server_promise.set_value(message);
    }));
    */
}

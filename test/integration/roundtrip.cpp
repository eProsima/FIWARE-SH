/*
 * Copyright 2019 - present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <is/sh/mock/api.hpp>

#include <is/core/Instance.hpp>

#include <is/utils/Log.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <iomanip>
#include <ctime>

#define FIWARE_IP      FIWARE__IP__TEST_CONFIG
#define FIWARE_PORT    FIWARE__PORT__TEST_CONFIG
#define FIWARE_ADDRESS FIWARE_IP ":" FIWARE_PORT

using namespace std::chrono_literals;

static eprosima::is::utils::Logger logger("is::sh::FIWARE::test");

namespace xtypes = eprosima::xtypes;

namespace eprosima {
namespace is {
namespace sh {
namespace fiware {
namespace test {

enum class test_type {
    basic_type,
    complex_type,
};

std::string gen_config_yaml(
        const std::string& topic_type_name,
        const std::string& fiware_entity,
        const std::string& topic_sent,
        const std::string& topic_recv,
        const test_type e = test_type::basic_type
        )
{
    std::string s;

    switch (e)
    {
        case test_type::basic_type:
            s += "types:\n";
            s += "    idls:\n";
            s += "        - >\n";
            s += "            struct " + topic_type_name + " {\n";
            s += "                string data;\n";
            s += "            };\n";
        break;
        case test_type::complex_type:
            s += "types:\n";
            s += "    idls:\n";
            s += "        - >\n";
            s += "            struct " + topic_type_name + " {\n";
            s += "                float x;\n";
            s += "                float y;\n";
            s += "                float z;\n";
            s += "                float w;\n";
            s += "            };\n";
        break;
    };

    s += "systems:\n";
    s += "    fiware: { type: fiware, host: \"" FIWARE_IP "\", port: " FIWARE_PORT "}\n";
    s += "    mock: { type: mock, types-from: fiware }\n";

    s += "routes:\n";
    s += "    mock_to_fiware: { from: mock, to: fiware }\n";
    s += "    fiware_to_mock: { from: fiware, to: mock }\n";

    s += "topics:\n";
    s += "    " + topic_sent + ": { type: \"" + topic_type_name +
            "\", route: mock_to_fiware, \
                    remap: { fiware: { topic: \"" + fiware_entity + "\" } } } \n";
    s += "    " + topic_recv + ": { type: \"" + topic_type_name +
            "\", route: fiware_to_mock, \
                    remap: { fiware: { topic: \"" + fiware_entity + "\" } } } \n";

    logger << utils::Logger::Level::INFO
           << "Using YAML file: \n" << s << std::endl;

    return s;
}

is::core::InstanceHandle load_test(
        const std::string& fiware_entity,
        const std::string& topic_type_name,
        const std::string topic_sent,
        const std::string topic_recv,
        const test_type e = test_type::basic_type)
{
    std::string config_yaml = gen_config_yaml(topic_type_name, fiware_entity, topic_sent, topic_recv, e);
    const YAML::Node config_node = YAML::Load(config_yaml);
    is::core::InstanceHandle is_handle = is::run_instance(config_node);

    std::this_thread::sleep_for(1s); //skip the first message from fiware, possibily from previous tests

    return is_handle;
}

void roundtrip(
        const std::string topic_sent,
        const std::string topic_recv,
        const xtypes::DynamicData& message,
        xtypes::DynamicData& result)
{
    is::sh::mock::publish_message(topic_sent, message);

    std::promise<xtypes::DynamicData> receive_msg_promise;
    ASSERT_TRUE(is::sh::mock::subscribe(topic_recv,
            [&](const xtypes::DynamicData& msg_from_fiware)
            {
                receive_msg_promise.set_value(msg_from_fiware);
            }));

    auto receive_msg_future = receive_msg_promise.get_future();
    ASSERT_EQ(std::future_status::ready, receive_msg_future.wait_for(5s));

    result = receive_msg_future.get();
}

bool create_fiware_entity(
        const std::string& entity,
        const std::string& type)
{
    const std::string create_entity_command =
            "curl " FIWARE_ADDRESS "/v2/entities -s -S -H \"Content-Type: application/json\" -d"
            "\"{"
            "    \\\"id\\\": \\\"" + entity + "\\\","
            "    \\\"type\\\": \\\"" + type + "\\\","
            "    \\\"data\\\": {"
            "        \\\"value\\\": \\\"\\\","
            "        \\\"type\\\": \\\"String\\\""
            "    }"
            "}\"";

    return 0 == system(create_entity_command.c_str());
}

bool delete_fiware_entity(
        const std::string& entity,
        const std::string& type)
{
    const std::string delete_entity_command =
            "curl " FIWARE_ADDRESS "/v2/entities/" + entity + "?type=" + type +
            " -s -S -H \
        \"Accept: application/json\" -X DELETE";

    return 0 == system(delete_entity_command.c_str());
}

std::string generate_msg()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

    logger << utils::Logger::Level::INFO
           << "Generated message ID: '" << ss.str() << "'" << std::endl;

    return " Mock test message at " + ss.str();
}

TEST(FIWARE, Transmit_to_and_receive_from_fiware__basic_type)
{
    const std::string fiware_entity = "fiware_mock_test_basic";
    const std::string topic_type_name = "TestType";
    const std::string topic_sent = "mock_to_fiware_topic";
    const std::string topic_recv = "fiware_to_mock_topic";

    //Remove previous instance if exists
    delete_fiware_entity(fiware_entity, topic_type_name);

    // The context broker entity will be created on subscription

    is::core::InstanceHandle is_handle =
            load_test(fiware_entity, topic_type_name, topic_sent, topic_recv);
    ASSERT_TRUE(is_handle);

    const is::TypeRegistry* mock_types = is_handle.type_registry("mock");
    EXPECT_NE(nullptr, mock_types);

    std::string message_data = generate_msg();
    xtypes::DynamicData msg_to_fiware(*mock_types->at(topic_type_name));
    msg_to_fiware["data"] = message_data;

    xtypes::DynamicData msg_from_fiware(*mock_types->at(topic_type_name));
    roundtrip(topic_sent, topic_recv, msg_to_fiware, msg_from_fiware);
    ASSERT_EQ(msg_to_fiware, msg_from_fiware);

    ASSERT_EQ(0, is_handle.quit().wait_for(1s));

    ASSERT_TRUE(delete_fiware_entity(fiware_entity, topic_type_name));
}

TEST(FIWARE, Transmit_to_and_receive_from_fiware__complex_type)
{
    const std::string fiware_entity = "fiware_mock_test_basic";
    const std::string topic_type_name = "TestType";
    const std::string topic_sent = "mock_to_fiware_topic";
    const std::string topic_recv = "fiware_to_mock_topic";

    //Remove previous instance if exists
    delete_fiware_entity(fiware_entity, topic_type_name);

    // The context broker entity will be created on subscription

    is::core::InstanceHandle is_handle =
            load_test(fiware_entity, topic_type_name, topic_sent, topic_recv, test_type::complex_type);
    ASSERT_TRUE(is_handle);

    const is::TypeRegistry* mock_types = is_handle.type_registry("mock");
    EXPECT_NE(nullptr, mock_types);

    xtypes::DynamicData msg_to_fiware(*mock_types->at(topic_type_name));
    msg_to_fiware["x"] = 0.1f;
    msg_to_fiware["y"] = 0.2f;
    msg_to_fiware["z"] = 0.3f;
    msg_to_fiware["w"] = 1.0f;

    xtypes::DynamicData msg_from_fiware(*mock_types->at(topic_type_name));
    roundtrip(topic_sent, topic_recv, msg_to_fiware, msg_from_fiware);
    ASSERT_EQ(msg_to_fiware, msg_from_fiware);

    ASSERT_EQ(0, is_handle.quit().wait_for(1s));

    ASSERT_TRUE(delete_fiware_entity(fiware_entity, topic_type_name));
}

} //  namespace test
} //  namespace fiware
} //  namespace sh
} //  namespace is
} //  namespace eprosima

int main(
        int argc,
        char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
